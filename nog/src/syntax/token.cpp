#include "nog/syntax/token.h"

#include <ostream>

namespace nog {

    std::ostream& operator<<(std::ostream& output, TokenKind kind) { return output; }

    std::ostream& operator<<(std::ostream& output, KeywordKind kind) { return output; }
} // namespace nog
