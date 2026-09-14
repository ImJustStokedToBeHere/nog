
#pragma once
#include <concepts>
#include <type_traits>

namespace nog {

    template <std::integral IntType>
    constexpr bool contains_any_flags(IntType val_or_set, IntType set) {
        return (val_or_set & set) > 0;
    }

    template <std::integral IntType>
    constexpr bool flag_is_set(IntType val, IntType set) {
        return (val & set) == val;
    }

    template <std::integral IntType>
    constexpr bool flag_is_not_set(IntType val, IntType set_of_val) {
        return (val & set_of_val) != val;
    }

    template <std::integral IntType>
    constexpr IntType set_flag(IntType val, IntType set_of_val) {
        return set_of_val |= val;
    }

    template <std::integral IntType>
    constexpr IntType unset_flag(IntType val, IntType set_of_val) {
        return set_of_val &= ~val;
    }

    template <std::integral IntType>
    constexpr IntType toggle_flag(IntType val, IntType set_of_val) {
        return set_of_val ^= val;
    }

    template <typename EnumType, std::enable_if_t<std::is_enum_v<EnumType>, int> = 0>
    constexpr bool contains_any_flags(EnumType val_or_set, EnumType set) {

        return contains_any_flags(static_cast<std::underlying_type_t<EnumType>>(val_or_set),
                                  static_cast<std::underlying_type_t<EnumType>>(set));
    }

    template <typename EnumType, std::enable_if_t<std::is_enum_v<EnumType>, int> = 0>
    constexpr bool flag_is_set(EnumType val, EnumType set) {
        return (val & set) == val;
    }

    template <typename EnumType, std::enable_if_t<std::is_enum_v<EnumType>, int> = 0>
    constexpr bool flag_is_not_set(EnumType val, EnumType set_of_val) {
        return (val & set_of_val) != val;
    }

    template <typename EnumType, std::enable_if_t<std::is_enum_v<EnumType>, int> = 0>
    constexpr EnumType set_flag(EnumType val, EnumType set_of_val) {
        return set_of_val |= val;
    }

    template <typename EnumType, std::enable_if_t<std::is_enum_v<EnumType>, int> = 0>
    constexpr EnumType unset_flag(EnumType val, EnumType set_of_val) {
        return set_of_val &= ~val;
    }

    template <typename EnumType, std::enable_if_t<std::is_enum_v<EnumType>, int> = 0>
    constexpr EnumType toggle_flag(EnumType val, EnumType set_of_val) {
        return set_of_val ^= val;
    }
} // namespace nog
