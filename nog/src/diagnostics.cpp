#include "nog/diagnostics.h"

namespace nog::dx {

    std::string error_msg(StatErr ec) {
        switch (ec) {
            case StatErr::InvalidCounterOpOnTimer:
                return "invalid counter operation on timer";
            case StatErr::InvalidTimerOpOnCounter:
                return "invalid timer operation on counter";
            default:
                return "unregistered StatErr";
        }
    }

} // namespace nog::dx
