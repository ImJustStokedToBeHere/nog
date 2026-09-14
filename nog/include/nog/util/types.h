#pragma once
#include <type_traits>
namespace nog {
    template <typename T>
    concept IsIntegerType = std::is_integral_v<T>;
} // namespace nog
