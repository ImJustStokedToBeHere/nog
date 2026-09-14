#include "nog/nog.h"
#include "nog/syntax/symbol.h"

#include <iostream>
#include <ostream>
#include <string>
#include <string_view>

using nog::Symbol;
using nog::SymbolCache;

int main(int argc, char** argv) {
    auto tester_result = nog::tester();

    std::string s = "hello there";
    SymbolCache cache;

    Symbol sym = cache.cache_string(s);
    std::cout << "string to cache: " << s << std::endl;
    std::cout << "symbol: " << sym.index() << std::endl;
    auto found_cached = cache.get_cached_string(sym);
    std::cout << "got cached str: " << (found_cached.has_value() ? found_cached.value() : "cached value not found")
              << std::endl;

    return 1;
}
