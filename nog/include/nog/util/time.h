
#pragma once
#include <cassert>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <string>

namespace nog {
    using Timestamp = std::chrono::time_point<std::chrono::system_clock>;
    using SystemClock = std::chrono::system_clock;
    using Filetime = std::filesystem::file_time_type;
    using Datetime = Timestamp;
    // typedef std::chrono::time_point<std::chrono::system_clock> clock_time;

    static std::chrono::seconds seconds_since_unix_epoch() {
        auto now = SystemClock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::seconds>(duration);
    }

    // awesome function, thanks Gulrak
    // https://stackoverflow.com/questions/61030383/how-to-convert-stdfilesystemfile-time-type-to-time-t
    template <typename TP>
    std::time_t to_time_t(TP tp) {
        // using namespace std::chrono;
        auto sctp = std::chrono::time_point_cast<SystemClock::duration>(tp - TP::clock::now()
                                                                        + SystemClock::now());

        return SystemClock::to_time_t(sctp);
    }

    std::string format_time_point(const std::string& format, Timestamp t);

    std::string iso_8601(Timestamp t, bool append_fractional_seconds = true);

    inline Timestamp timestamp() { return SystemClock::now(); }

    inline std::string timestamp_str(bool append_fractional_seconds) {
        return iso_8601(timestamp(), append_fractional_seconds);
    }

} // namespace nog
