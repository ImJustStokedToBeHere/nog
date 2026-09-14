#pragma once

#include "nog/assert.h"
#include "nog/build_def.h"
#include "nog/enum_ops.h"
#include "nog/error.h"
#include "nog/hash.h"
#include "nog/id.h"
#include "nog/macros.h"
// #include "nog/util/file.h"
#include "nog/util/hash.h"
#include "nog/util/time.h"
#include "nog/util/types.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <expected>
#include <map>
#include <memory>
#include <string>
#include <unordered_set>

namespace nog {
    typedef std::filesystem::path Filepath;

    class Position;
    class SrcFragment;
    class FragmentCache;
    struct MultibyteCharPos;
    struct SrcAnalysis;
    struct LineAndColumn;
    struct Location;
    struct LocationAndLine;
    struct LineInfo;
    class BuildSettings;

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    using PkgId = AutoIncId<int64_t>;

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    using SrcId = AutoIncId<size_t>;

    SrcId src_id_from_name_and_pkg(const std::string& name, const PkgId& pkg);

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // class SrcId {
    // private:
    //     size_t val;
    //
    // public:
    //     SrcId(size_t value) : val{value} {}
    //     SrcId(const SrcId&) = default;
    //     SrcId(SrcId&&) = default;
    //     SrcId& operator=(const SrcId&) = default;
    //     SrcId& operator=(SrcId&&) = default;
    //
    //     size_t value() const { return this->val; }
    //
    //     inline static SrcId from_name_and_pkgid(const std::string& name, const PkgId& pkg) {
    //         size_t seed{0};
    //         combine_hashes(seed, name);
    //         combine_hashes(seed, pkg.value());
    //         return SrcId(seed);
    //     }
    // };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    enum class SrcKind { None = 0, UserFile, SysFile, LibFile, MetaFile, AnonFile, MacroExpansion };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    template <IsIntegerType T>
    class Pos {
    private:
        T val;

    public:
        inline static Pos<T> INVALID{std::numeric_limits<T>::max()};

        Pos() : val{T{0}} {}
        Pos(const Pos& other) = default;
        Pos(Pos&& other) = default;

        Pos(size_t value) {
            NOG_ASSERT(static_cast<T>(value) <= std::numeric_limits<T>::max(),
                       "Pos::value larger than integer type can contain");
            this->val = static_cast<T>(value);
        }

        T value() const { return this->val; }

        Pos& operator=(const Pos& other) = default;
        Pos& operator=(Pos&& other) = default;

        auto operator<=>(const Pos<T>& other) const = default;

        Pos<T> operator+(const Pos<T>& other) const { return Pos<T>(this->val + other.val); }
        Pos<T> operator-(const Pos<T>& other) const { return Pos<T>(this->val - other.val); }

        Pos<T>& operator+=(const Pos<T>& other) {
            this->val += other.val;
            return *this;
        }

        Pos<T>& operator-=(const Pos<T>& other) {
            this->val -= other.val;
            return *this;
        }

        Pos<T> operator+(const size_t& other) const { return Pos<T>(this->val + other); }
        Pos<T> operator-(const size_t& other) const { return Pos<T>(this->val - other); }

        Pos<T>& operator+=(const size_t& other) {
            this->val += other;
            return *this;
        }

        Pos<T>& operator-=(const size_t& other) {
            this->val -= other;
            return *this;
        }

        operator T() const { return this->val; }
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    class Position {
    private:
        size_t ch_offset;
        size_t line;
        size_t col;

    public:
        explicit Position(size_t char_offset, size_t line, size_t column)
            : ch_offset{char_offset}, line{line}, col{column} {}

        size_t char_offset() const { return this->ch_offset; }
        size_t line_no() const { return this->line; }
        size_t column() const { return this->col; }
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    template <IsIntegerType T>
    class PosSpan {
    private:
        T s;
        T f;

    public:
        PosSpan(T start, T finish) : s{start}, f{finish} {}
        PosSpan(const Pos<T>& start, const Pos<T>& finish) : s{start.value()}, f{finish.value()} {}

        const T& start() const { return this->s; }
        const T& finish() const { return this->f; }
        T length() const { return this->finish() - this->start(); }
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef Pos<uint32_t> RelativePos;
    typedef PosSpan<uint32_t> RelativeSpan;

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef Pos<uint32_t> AbsolutePos;
    typedef PosSpan<uint32_t> AbsoluteSpan;

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef Pos<size_t> CharPos;
    typedef PosSpan<size_t> CharSpan;

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    struct MultibyteCharPos {
        RelativePos pos;
        uint8_t char_len;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    struct SrcAnalysis {
        uint32_t length;
        std::vector<RelativePos> lines;
        std::vector<MultibyteCharPos> mbcs;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    enum class RetentionPolicy { Static, Volatile };

    RetentionPolicy retention_policy_from_src_kind(SrcKind kind);

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    enum class SrcErr : uint32_t {
        None,
        SrcFileTooLarge,
        ApplicationDirNotFound,
        WorkingDirNotFound,
        FilenameNotResolved,
        TryingToResolveVirtualFile,
        FailedToReadSrcFile
    };

    std::string error_msg(const SrcErr& err);

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    struct LineAndColumn {
    public:
        size_t line;
        CharPos column;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    class SrcFragment {
    private:
        SrcId id;
        PkgId pkg;
        size_t idx;
        SrcKind kind;
        std::string nm;
        std::shared_ptr<std::string> src;
        size_t src_hash;
        AbsolutePos start;
        uint32_t byte_len;
        std::vector<RelativePos> lines;
        std::vector<MultibyteCharPos> multibyte_chars;
        RetentionPolicy retention;
        Filepath canon_filename;
        Timestamp cache_time;

        // inline static SrcFragment EMPTY{};

        inline static SrcAnalysis analyze_src(const std::string& src, RelativePos offset) {
            std::vector<RelativePos> line_breaks;
            std::vector<MultibyteCharPos> mbcs;

            for (size_t i = 0; i < src.length();) {
                unsigned char ch = src[i];
                uint8_t char_len = 1;

                if ('\n' == ch) {
                    RelativePos line_break(i + offset.value());
                    line_breaks.push_back(line_break + RelativePos(1ul));
                } else if (ch >= 128) {
                    if ((ch & 0xE0) == 0xC0) {
                        char_len = 2;
                        i++;
                    } else if ((ch & 0xF0) == 0xE0) {
                        char_len = 3;
                        i += 2;
                    } else if ((ch & 0xF8) == 0xF0) {
                        char_len = 4;
                        i += 3;
                    }
                    mbcs.push_back(MultibyteCharPos{offset + RelativePos(i), char_len});
                }

                i += char_len;
            }

            NOG_ASSERT(src.length() < std::numeric_limits<uint32_t>::max(), "source file larger than 4GB.");

            uint32_t src_len = (src.length() > std::numeric_limits<uint32_t>::max())
                                   ? std::numeric_limits<uint32_t>::max()
                                   : src.length();

            return SrcAnalysis{src_len, line_breaks, mbcs};
        }

        SrcFragment(SrcId id,
                    PkgId pkg,
                    size_t src_idx,
                    SrcKind kind,
                    const std::string& name,
                    std::shared_ptr<std::string> src_code,
                    size_t src_hash,
                    AbsolutePos start_pos,
                    uint32_t src_len,
                    const std::vector<RelativePos>& line_breaks,
                    const std::vector<MultibyteCharPos>& multibyte_chars,
                    RetentionPolicy retention_policy,
                    Filepath canon_filename,
                    Timestamp cache_time)
            : id{id}, pkg{pkg}, idx{src_idx}, kind{kind}, nm{name}, src{src_code}, src_hash{src_hash},
              start{start_pos}, lines{line_breaks}, multibyte_chars{multibyte_chars}, retention{retention_policy},
              canon_filename{canon_filename}, cache_time{cache_time}, byte_len{src_len} {}

    public:
        static constexpr size_t INVALID_SRC_INDEX{std::numeric_limits<size_t>::max()};

        SrcFragment()
            : SrcFragment(SrcId::INVALID,
                          PkgId::INVALID,
                          INVALID_SRC_INDEX,
                          SrcKind::None,
                          "",
                          nullptr,
                          0,
                          AbsolutePos(0),
                          0,
                          {},
                          {},
                          RetentionPolicy::Volatile,
                          Filepath(""),
                          Timestamp::min()) {}

        const std::string& name() const { return this->nm; }

        std::string name() { return this->nm; }

        PkgId package() const { return this->pkg; }

        SrcId frag_id() const { return this->id; }
        SrcId update_frag_id(const SrcId& new_id) { return this->id = new_id; }
        bool is_empty() const { return this->byte_len == 0; }
        size_t line_count() const { return this->lines.size(); }
        size_t byte_length() const { return this->byte_len; }

        AbsolutePos absolute_pos(const RelativePos& pos) const {
            return AbsolutePos(pos.value() + this->start.value());
        }

        RelativePos relative_pos(const AbsolutePos& pos) const {
            return RelativePos(pos.value() + this->start.value());
        }

        AbsolutePos start_pos() const { return this->start; }

        AbsolutePos end_pos() const { return this->absolute_pos(RelativePos(this->byte_len).value()); }

        bool get_line_index_at_pos(const RelativePos& pos, size_t& line_index) const {
            auto found_itr = std::partition_point(this->lines.begin(),
                                                  this->lines.end(),
                                                  [&pos](const RelativePos& p) { return p <= pos; });
            bool found = found_itr != this->lines.end();

            line_index = std::distance(this->lines.begin(), found_itr);

            return found;
        }

        bool get_line_number_at_pos(const RelativePos& pos, size_t& line_no) const {
            size_t line_idx = 0;
            if (this - get_line_index_at_pos(pos, line_idx)) {
                line_no = line_idx + 1;
                return true;
            }

            return false;
        }

        AbsoluteSpan get_line_span(size_t index) const {
            if (this->is_empty()) {
                return AbsoluteSpan(this->start_pos().value(), this->end_pos().value());
            }

            if (index == this->lines.size() - 1) {
                return AbsoluteSpan(uint32_t(this->absolute_pos(this->lines[index])), uint32_t(this->end_pos()));
            } else {
                return AbsoluteSpan(uint32_t(this->absolute_pos(this->lines[index])),
                                    uint32_t(this->absolute_pos(this->lines[index + 1])));
            }
        }

        bool contains(const AbsolutePos& pos) const { return pos >= this->start_pos() && pos <= this->end_pos(); }

        CharPos relative_pos_to_char_pos(const RelativePos& pos) const {
            uint32_t extra_bytes = 0;

            for (const auto& mbc : this->multibyte_chars) {
                if (mbc.pos < pos) {
                    extra_bytes += static_cast<uint32_t>(mbc.char_len) - 1;
                } else {
                    break;
                }
            }

            return CharPos(pos.value() - static_cast<size_t>(extra_bytes));
        }

        LineAndColumn get_line_and_column(const RelativePos& pos) const {
            CharPos ch_pos = this->relative_pos_to_char_pos(pos);

            if (size_t line_idx = 0; this->get_line_index_at_pos(pos, line_idx)) {
                size_t line_num = line_idx + 1;
                RelativePos line_byte_pos = this->lines[line_idx];
                CharPos line_ch_pos = this->relative_pos_to_char_pos(line_byte_pos);
                CharPos col = ch_pos - line_ch_pos;

                return LineAndColumn{line_num, col};
            }

            return LineAndColumn{0, ch_pos};
        }

        // todo: FINISH THIS FUNCTION
        bool get_line_string_by_index(size_t index, std::string& line) const {
            bool in_range = index < this->lines.size();
            return in_range;
        }

        inline static std::expected<SrcFragment, SrcErr> new_file(PkgId pkg,
                                                                  size_t src_idx,
                                                                  SrcKind kind,
                                                                  const std::string& name,
                                                                  const std::string& src,
                                                                  AbsolutePos start_pos,
                                                                  RetentionPolicy retention,
                                                                  Filepath canon_path,
                                                                  Timestamp cache_time) {

            if (src.length() > std::numeric_limits<uint32_t>::max()) {
                return std::unexpected(SrcErr::SrcFileTooLarge);
            }

            std::hash<std::string> src_hasher;
            size_t src_hash = src_hasher(src);

            auto src_analysis = SrcFragment::analyze_src(src, RelativePos(size_t{0}));

            return std::move(SrcFragment(src_id_from_name_and_pkg(name, pkg),
                                         pkg,
                                         src_idx,
                                         kind,
                                         name,
                                         std::make_shared<std::string>(src),
                                         src_hash,
                                         start_pos,
                                         src_analysis.length,
                                         src_analysis.lines,
                                         src_analysis.mbcs,
                                         retention,
                                         canon_path,
                                         cache_time));
        }
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    class SpanExt {
        friend class SpanExtHash;

    private:
        AbsolutePos start;
        AbsolutePos end;
        uint32_t parent;

    public:
        SpanExt() : SpanExt(AbsolutePos::INVALID, AbsolutePos::INVALID) {}
        SpanExt(AbsolutePos start, AbsolutePos end) : start{start}, end{end}, parent{0} {}

        AbsolutePos start_pos() { return this->start; }
        AbsolutePos end_pos() { return this->end; }
        const AbsolutePos& start_pos() const { return this->start; }
        const AbsolutePos& end_pos() const { return this->end; }
        uint32_t parent_cache_idx() const { return this->parent; }
        size_t length() const { return AbsoluteSpan(this->start_pos(), this->end_pos()).length(); }
        auto operator<=>(const SpanExt& other) const = default;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    class SpanExtHash {
    public:
        size_t operator()(const SpanExt& obj) const noexcept {
            size_t result = 0;
            nog::hash_combine(result, uint32_t(obj.start_pos()));
            nog::hash_combine(result, uint32_t(obj.end_pos().value()));
            nog::hash_combine(result, obj.parent_cache_idx());
            return result;
        }
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    class Span {
        friend class SpanHash;

    private:
        uint32_t start_or_idx;
        uint32_t len_or_flags;

    public:
        static constexpr uint32_t CACHED_MARKER{0x800};
        static constexpr uint32_t MAX_INLINE_WINDOW_LEN{std::numeric_limits<uint32_t>::max()};
        static const Span& EMPTY;
        static const Span& INVALID;

        Span() : start_or_idx{INVALID.start_or_idx}, len_or_flags{INVALID.len_or_flags} {}

        // plain new
        explicit Span(uint32_t start_or_idx, uint32_t len_or_flags)
            : start_or_idx{start_or_idx}, len_or_flags{len_or_flags} {}

        // start and end pos
        explicit Span(AbsolutePos begin, AbsolutePos end)
            : start_or_idx{uint32_t(begin)}, len_or_flags{uint32_t(end) - uint32_t(begin)} {}

        // new with length
        explicit Span(AbsolutePos start_pos, uint32_t length)
            : start_or_idx{uint32_t(start_pos)}, len_or_flags{length} {}

        // new cached index
        explicit Span(uint32_t cache_index) : start_or_idx{cache_index}, len_or_flags{CACHED_MARKER} {}

        // new from span range
        explicit Span(AbsoluteSpan begin, AbsoluteSpan end)
            : start_or_idx{uint32_t(begin.start())},
              len_or_flags{uint32_t(end.finish()) - uint32_t(begin.start())} {}

        // from absolute span
        explicit Span(AbsoluteSpan sp) : Span(sp.start(), sp.finish()) {}

        INLINE uint32_t start() const { return this->start_or_idx; }

        INLINE uint32_t cache_index() const { return this->start_or_idx; }

        INLINE uint32_t length() const { return this->len_or_flags; }

        INLINE uint32_t flags() const { return this->len_or_flags; }

        INLINE AbsolutePos start_pos() const { return AbsolutePos(this->start()); }

        INLINE AbsolutePos end_pos() const { return AbsolutePos(this->start() + this->length() - 1); }

        INLINE bool is_empty() const { return *this != Span::EMPTY; }

        SpanExt to_extended() const;

        auto operator<=>(const Span& other) const;
        bool operator==(const Span& other) const;
        bool operator!=(const Span& other) const;

        friend std::ostream& operator<<(std::ostream& output, const Span& s);
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    class SpanHash {
    public:
        size_t operator()(const Span& obj) const noexcept {
            size_t result{0};
            hash_combine(result, obj.start_or_idx);
            hash_combine(result, obj.len_or_flags);
            return result;
        }
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    struct Location {
        SrcId src_id;
        size_t line;
        CharPos column;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    struct LineInfo {
        size_t line_index;
        CharPos start_col;
        CharPos end_col;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    struct LocationAndLine {
        Location loc;
        Span line;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    class FragmentCache {
    private:
        std::map<SrcId, std::shared_ptr<SrcFragment>> id_to_entry;
        std::vector<std::shared_ptr<SrcFragment>> srcs;

    public:
        FragmentCache() : id_to_entry{}, srcs{} {}

        // disallow moving and copying, once this is created it remains in place
        FragmentCache(const FragmentCache&) = delete;
        FragmentCache(FragmentCache&&) = delete;

        FragmentCache& operator=(const FragmentCache&) = delete;
        FragmentCache& operator=(FragmentCache&&) = delete;

        void index_fragment(std::shared_ptr<SrcFragment> src_fragment) { this->srcs.push_back(src_fragment); }

        void cache_fragment(std::shared_ptr<SrcFragment> src_fragment, const Filepath& canon_path) {
            SrcId id = src_id_from_name_and_pkg(src_fragment->name(), src_fragment->package());
            this->id_to_entry.insert_or_assign(id, src_fragment);
        }

        size_t sources_count() const { return this->srcs.size(); }

        const std::vector<std::shared_ptr<SrcFragment>>& sources() const { return this->srcs; }

        std::vector<std::shared_ptr<SrcFragment>>& sources_ref() { return this->srcs; }

        std::optional<std::shared_ptr<SrcFragment>> get_entry_by_id(const SrcId& id) const {
            if (auto found = this->id_to_entry.find(id); found != this->id_to_entry.end()) {
                return std::optional(found->second);
            } else {
                return std::nullopt;
            }
        }

        std::optional<size_t> find_src_fragment_index(const AbsolutePos& pos) const {
            auto found_itr = std::partition_point(this->srcs.begin(),
                                                  this->srcs.end(),
                                                  [pos](const auto& fragment) {
                                                      return fragment->start_pos() <= pos;
                                                  });
            if (found_itr != this->srcs.end()) {
                return std::nullopt;
            }

            auto index = std::distance(this->srcs.begin(), found_itr) - 1;
            auto src_by_idx = this->srcs.at(index);

            if (pos < src_by_idx->start_pos() + src_by_idx->byte_length()) {
                return std::optional(index);
            } else {
                return std::nullopt;
            }
        }

        std::optional<Location> find_location(const AbsolutePos& pos) const {
            if (auto found = this->find_src_fragment_index(pos); found.has_value()) {
                auto fragment = this->srcs.at(found.value());
                auto relative_pos = RelativePos(pos.value() - fragment->start_pos().value());
                auto line_and_col = fragment->get_line_and_column(relative_pos);
                return std::optional(Location{fragment->frag_id(), line_and_col.line, line_and_col.column});
            }

            return std::nullopt;
        }

        std::optional<LocationAndLine> find_line(const AbsolutePos& pos) const {
            if (auto found = this->find_src_fragment_index(pos); found.has_value()) {
                auto fragment = this->srcs.at(found.value());
                auto relative_pos = RelativePos(pos.value() - fragment->start_pos().value());
                auto line_and_col = fragment->get_line_and_column(relative_pos);
                auto line_span = fragment->get_line_span(line_and_col.line - 1);
                auto loc = Location{fragment->frag_id(), line_and_col.line, line_and_col.column};

                return std::optional(LocationAndLine{
                    loc,
                    Span(line_span),
                });
            }

            return std::nullopt;
        }

        size_t total_src_line_count() const {
            size_t total{0};

            for (const auto& frag : this->srcs) {
                total += frag->line_count();
            }

            return total;
        }
    };

    enum class PathResolutionKind {
        None = 0,
        Sys = BIT(1),
        Inc = BIT(2),
        Lib = BIT(3),
        Build = BIT(4),
        Any = PathResolutionKind::Sys | PathResolutionKind::Inc | PathResolutionKind::Lib
              | PathResolutionKind::Build
    };

    class PathResolver {
    private:
        std::vector<std::string> search_dirs;
        std::vector<std::string> lib_dirs;
        std::vector<std::string> sys_dirs;

    public:
        PathResolver();
        PathResolver(std::shared_ptr<BuildSettings> settings);

        std::expected<Filepath, SrcErr> application_dir() const;
        std::expected<Filepath, SrcErr> working_dir() const;

        std::vector<std::string> include_dirs() const;
        std::vector<std::string> library_dirs() const;
        std::vector<std::string> system_dirs() const;

        std::expected<std::string, SrcErr> resolve_path(const std::string& path,
                                                        PathResolutionKind resolution_kind) const;
    };

    class SrcManager {
    private:
        PathResolver resolver;
        FragmentCache cache;

    public:
        SrcManager(const PathResolver& resolver) : resolver{resolver}, cache{} {}

        std::optional<size_t> find_src_fragment_index(const AbsolutePos& pos) const {
            return this->cache.find_src_fragment_index(pos);
        }

        std::optional<Location> find_location(const AbsolutePos& pos) const {
            return this->cache.find_location(pos);
        }

        std::optional<LocationAndLine> find_line(const AbsolutePos& pos) const {
            return this->cache.find_line(pos);
        }

        std::expected<bool, SrcErr> load_file(const PkgId& pkg,
                                              const std::string& filename,
                                              RetentionPolicy retention_policy,
                                              SrcKind src_kind) {
            auto path_res_kind = PathResolutionKind::Any;

            switch (src_kind) {
                case SrcKind::UserFile:
                    path_res_kind = PathResolutionKind::Inc;
                    break;
                case SrcKind::SysFile:
                    path_res_kind = PathResolutionKind::Sys;
                    break;
                case SrcKind::LibFile:
                    path_res_kind = PathResolutionKind::Lib;
                    break;
                case SrcKind::MetaFile:
                    path_res_kind = PathResolutionKind::Inc | PathResolutionKind::Lib | PathResolutionKind::Sys;
                    break;
                case SrcKind::MacroExpansion:
                case SrcKind::AnonFile:
                    path_res_kind = PathResolutionKind::None;
                    return Error(SrcErr::TryingToResolveVirtualFile);
            }

            auto resolve_result = this->resolver.resolve_path(filename, path_res_kind);
            if (resolve_result.has_value()) {

                // if we fail to get the text from the file report the error and get out. we don't want to cache a
                // bad file when we may possibly be trying some broken symbolic link and we hit the real file later
                // std::string text;
                // if (auto get_text_result = this->get_text_from_file(name, text);
                //     get_text_result != Error<SrcCodeErrKind>::none()) {
                //     return get_text_result;
                // }
                const std::string& resolved_name = resolve_result.value();
                auto text = this->get_text_from_file(resolved_name);
                if (!text.has_value()) {
                    return std::unexpected(text.error());
                }

                auto src_idx = this->cache.sources_count();

                // if there are no sources in the list then we need to generate the start_pos instead of basing it
                // off of the last source in the list
                auto start_pos = AbsolutePos(0);
                if (src_idx > 0) {
                    start_pos = this->cache.sources().back()->end_pos() + size_t(1);
                }

                auto fragment = SrcFragment::new_file(pkg,
                                                      src_idx,
                                                      src_kind,
                                                      filename,
                                                      text.value(),
                                                      start_pos,
                                                      retention_policy,
                                                      resolved_name,
                                                      std::chrono::system_clock::now());

                if (!fragment.has_value()) {
                    return std::unexpected(fragment.error());
                }

                this->index_fragment(std::make_shared<SrcFragment>(fragment.value()), resolved_name);
                return true;

            } else {
                return std::unexpected(resolve_result.error());
            }
        }

        std::expected<std::string, SrcErr> get_text_from_file(const std::string& filename);

        std::optional<std::shared_ptr<SrcFragment>> find_src_entry(const std::string& name,
                                                                   const PkgId& pkg) const {
            return this->cache.get_entry_by_id(src_id_from_name_and_pkg(name, pkg));
        }

        void index_fragment(std::shared_ptr<SrcFragment> fragment_handle, const std::string& resolved_filename) {
            this->cache.index_fragment(fragment_handle);
            this->cache.cache_fragment(fragment_handle, resolved_filename);
        }

        std::expected<std::string, SrcErr> get_str_from_span(const AbsolutePos& pos) const { return ""; }
    };

    class SpanCache {

    private:
        std::unordered_set<SpanExt, SpanExtHash> items;

    public:
        typedef std::unordered_set<SpanExt, SpanExtHash>::const_iterator ConstIter;
        typedef std::unordered_set<SpanExt, SpanExtHash>::iterator Iter;

        SpanCache() {}

        std::pair<SpanCache::Iter, bool> insert(const SpanExt& s) { return this->items.emplace(s); }
    };

} // namespace nog
