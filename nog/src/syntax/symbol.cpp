#include "nog/syntax/symbol.h"

#include <limits>

namespace nog {
    Symbol Symbol::ANY{std::numeric_limits<uint32_t>::max() - 2};
    Symbol Symbol::EMPTY{std::numeric_limits<uint32_t>::max() - 1};
    Symbol Symbol::INVALID{std::numeric_limits<uint32_t>::max()};
} // namespace nog
