#pragma once
#include "nog/enum_ops.h"
#include "nog/flag_ops.h"
#include "nog/macros.h"
#include "nog/source.h"
#include "nog/util/print_helpers.h"
#include "nog/util/time.h"

#include <expected>
#include <map>
#include <string>

namespace nog::dx {

    enum class StatErr : uint32_t {
        None,
        InvalidCounterOpOnTimer,
        InvalidTimerOpOnCounter,
    };

    std::string error_msg(StatErr ec);

    struct Timer {
        Timestamp start;
        Timestamp finish;
    };

    enum class StatKind { Counter, Timer };

    class Stat {
    private:
        StatKind kind;
        union {
            size_t counter;
            Timer timer;
        };

        Stat(size_t counter_start_value) : kind{StatKind::Counter} { this->counter = counter_start_value; }
        Stat(Timestamp timer_start_value) : kind{StatKind::Timer} { this->timer.start = timer_start_value; }

    public:
        inline static Stat new_timer(Timestamp start_time = Timestamp::min()) { return Stat(start_time); }
        inline static Stat new_counter(size_t start_value = 0) { return Stat(start_value); }

        StatKind stat_kind() const { return this->kind; }

        // timer operations
        std::expected<Timestamp, StatErr> start_timer() {
            if (this->kind == StatKind::Timer) {
                this->timer.start = Timestamp::clock::now();
                return this->timer.start;
            }

            return std::unexpected(StatErr::InvalidTimerOpOnCounter);
        }

        std::expected<Timestamp, StatErr> stop_timer() {
            if (this->kind == StatKind::Timer) {
                this->timer.finish = Timestamp::clock::now();
                return this->timer.finish;
            }

            return std::unexpected(StatErr::InvalidTimerOpOnCounter);
        }

        template <typename Rep, typename Period>
        std::expected<std::chrono::duration<Rep, Period>, StatErr> duration() {
            if (this->kind == StatKind::Timer) {
                return this->timer.finish - this->timer.start;
            }

            return std::unexpected(StatErr::InvalidTimerOpOnCounter);
        }

        template <typename Rep, typename Period>
        std::expected<std::chrono::duration<Rep, Period>, StatErr> elapsed() {
            if (this->kind == StatKind::Timer) {
                return Timestamp::clock::now() - this->timer.start;
            }

            return std::unexpected(StatErr::InvalidTimerOpOnCounter);
        }

        std::expected<Timestamp, StatErr> start_time() const {
            if (this->kind == StatKind::Timer) {
                return this->timer.start;
            }

            return std::unexpected(StatErr::InvalidTimerOpOnCounter);
        }

        std::expected<Timestamp, StatErr> finish_time() const {
            if (this->kind == StatKind::Timer) {
                return this->timer.finish;
            }

            return std::unexpected(StatErr::InvalidTimerOpOnCounter);
        }

        // counter operations
        std::expected<size_t, StatErr> count() const {
            if (this->kind == StatKind::Counter) {
                return this->counter;
            }

            return std::unexpected(StatErr::InvalidCounterOpOnTimer);
        }

        std::expected<size_t, StatErr> inc() { return this->inc_by(1); }

        std::expected<size_t, StatErr> inc_by(size_t n) {
            if (this->kind == StatKind::Counter) {
                this->counter += n;
            }

            return std::unexpected(StatErr::InvalidCounterOpOnTimer);
        }

        std::expected<size_t, StatErr> dec() { return this->dec_by(1); }

        std::expected<size_t, StatErr> dec_by(size_t n) {
            if (this->kind == StatKind::Counter) {
                this->counter -= n;
            }

            return std::unexpected(StatErr::InvalidCounterOpOnTimer);
        }
    };

    class StatTracker {
    private:
        std::map<std::string, Stat> items;

    public:
        StatTracker() {}

        void new_timer(const std::string& name, const Timestamp& start_time = Timestamp::clock::now()) {
            this->items.insert_or_assign(name, Stat::new_timer(start_time));
        }

        void new_counter(const std::string& name, size_t start_value = 0) {
            this->items.insert_or_assign(name, Stat::new_counter(start_value));
        }

        void print_stats(std::ostream& output, DataFormat fmt) {
            switch (fmt) {
                case DataFormat::Json:
                    break;
                case DataFormat::Plain:
                    break;
                case DataFormat::Xml:
                    break;
                case DataFormat::Html:
                    break;
            }
        }
    };

    enum class InfoClass {
        None = 0,
        Fatal = BIT(0),
        Err = BIT(1),
        Warn = BIT(2),
        Suggest = BIT(3),
        Info = BIT(4),
        AllButFatal = Err | Warn | Suggest | Info,
        AllButErrors = Warn | Suggest | Info,
        InfoAndSuggest = Info | Suggest,
    };

    enum class DiagnosticId {};

    constexpr auto g = sizeof(std::shared_ptr<std::string>);

    class Diagnostic {
    private:
        DiagnosticId id;
        InfoClass iclass;
        std::unique_ptr<std::string> addendum;
        Span loc;
        Timestamp time;

    public:
        Diagnostic(DiagnosticId id,
                   InfoClass info_class,
                   Span location,
                   Timestamp time,
                   std::unique_ptr<std::string> addendum)
            : id{id}, iclass{info_class}, loc{location}, time{time}, addendum{std::move(addendum)} {}

        Diagnostic(const Diagnostic&) = delete;
        Diagnostic(Diagnostic&&) = default;

        InfoClass info_class() const { return this->iclass; }
        Span location() const { return this->loc; }
        DiagnosticId diag_id() const { return this->id; }

        Diagnostic& operator=(const Diagnostic&) = delete;
        Diagnostic& operator=(Diagnostic&&) = default;
    };

    struct DiagnosticBag {
        std::vector<Diagnostic> diags;
        std::map<DiagnosticId, std::vector<size_t>> lookup;
    };

    class DiagnosticTracker {
    private:
        InfoClass included_classes;
        DiagnosticBag items;
        StatTracker stats;

    public:
        DiagnosticTracker() : included_classes{InfoClass::None}, items{}, stats{} {}

        explicit DiagnosticTracker(const BuildSettings&) : DiagnosticTracker() {}

        //     std::vector<const Diagnostic*> get_diagnostic_for_file(const Span& file_span) {
        //
        // }
        //
        //     std::vector<const Diagnostic*> get_diagnostic_for_span(const Span& span) {}

        void print_diagnostics(std::ostream& output, DataFormat fmt) {
            switch (fmt) {
                case DataFormat::Json:
                    break;
                case DataFormat::Plain:
                    break;
                case DataFormat::Xml:
                    break;
                case DataFormat::Html:
                    break;
            }
        }

        void add_diagnostic(Diagnostic dx) {
            this->included_classes |= dx.info_class();
            this->items.diags.emplace_back(std::move(dx));
            size_t index = this->items.diags.size() - 1;

            // lets see if there is already one in there with this id
            if (this->items.lookup.contains(dx.diag_id())) {
                this->items.lookup[dx.diag_id()].push_back(index);
            } else {
                std::vector<size_t> indices;
                indices.push_back(index);
                this->items.lookup.emplace(dx.diag_id(), indices);
            }
        }

        size_t make_diagnostic(DiagnosticId id,
                               InfoClass infoclass,
                               Span location,
                               Timestamp time,
                               const std::string& addendum = "") {
            this->add_diagnostic(Diagnostic(id,
                                            infoclass,
                                            location,
                                            time,
                                            addendum == "" ? nullptr : std::make_unique<std::string>(addendum)));
            return this->items.diags.size() - 1;
        }

        bool contains_fatal_diag() const {
            return static_cast<size_t>(this->included_classes & InfoClass::Fatal) > 0;
        }

        std::vector<const Diagnostic*> get_diags_by_infoclass(InfoClass infoclass) const {
            // iterate the diags, take anything that bitwise matches the infoclass
            std::vector<const Diagnostic*> results;
            for (const auto& diag : this->items.diags) {
                if (contains_any_flags(diag.info_class(), infoclass)) {
                    results.push_back(&diag);
                }
            }

            return results;
        }
    };

    class DiagnosticEmitter {
    public:
        virtual ~DiagnosticEmitter() {}
        virtual void warn(DiagnosticId id) = 0;
        virtual void warn(DiagnosticId id, const std::string& msg) = 0;

        virtual void fatal(DiagnosticId id) = 0;
        virtual void fatal(DiagnosticId id, const std::string& msg) = 0;

        virtual void suggest(DiagnosticId id) = 0;
        virtual void suggest(DiagnosticId id, const std::string& msg) = 0;

        virtual void error(DiagnosticId id) = 0;
        virtual void error(DiagnosticId id, const std::string& msg) = 0;
    };

} // namespace nog::dx
