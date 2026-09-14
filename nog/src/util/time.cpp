
#include "nog/util/time.h"

#include "nog/build_def.h"

#include <time.h>

namespace nog {
    std::string format_time_point(const std::string& format, Timestamp t) { return std::format("{0:%F}", t); }

    std::string iso_8601(Timestamp t, bool append_fractional_seconds) {
        // convert to time_t which will represent the number of
        // seconds since the UNIX epoch, UTC 00:00:00 Thursday, 1st. January 1970
        struct tm buf;
        auto epoch_seconds = SystemClock::to_time_t(t);

        // Format this as date time to seconds resolution
        // e.g. 2016-08-30T08:18:51
        std::stringstream stream;
        if (t == Timestamp::min()) {
            return "invalid timepoint provided to std::string "
                   "iso_8601(std::chrono::time_point<std::chrono::system_clock> t,"
                   "bool append_fractional_seconds = true)";
        }

#if defined(_WIN32)
        auto result = std::gmtime_s(&buf, &epoch_seconds, );
#else
        auto result = gmtime_r(&epoch_seconds, &buf);
#endif

        // assert(result == 0 && "gmtime call failed. timer or buf is a null pointer");
        stream << std::put_time(&buf, "%FT%T");

        if (append_fractional_seconds) {
            // If we now convert back to a time_point we will get the time truncated
            // to whole seconds
            auto truncated = SystemClock::from_time_t(epoch_seconds);

            // Now we subtract this seconds count from the original time to
            // get the number of extra microseconds..
            auto delta_us = std::chrono::duration_cast<std::chrono::microseconds>(t - truncated).count();

            // And append this to the output stream as fractional seconds
            // e.g. 2016-08-30T08:18:51.867479
            stream << "." << std::fixed << std::setw(6) << std::setfill('0') << delta_us;
        }

        return stream.str();
    }
} // namespace nog
