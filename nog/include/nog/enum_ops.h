
#pragma once
#include <concepts>
#include <limits>
#include <stdint.h>
#include <type_traits>

namespace nog {

    constexpr uint8_t as_integer(uint8_t val) { return val; }

    template <typename EnumType,
              typename IntType,
              std::enable_if_t<std::is_integral_v<IntType>, int> = 0,
              std::enable_if_t<std::is_enum_v<EnumType>, int> = 0>
    constexpr IntType as_integer(EnumType val) {
        return static_cast<IntType>(val);
    }

    template <typename EnumType, std::enable_if_t<std::is_enum_v<EnumType>, int> = 0>
    constexpr auto as_integer(EnumType val) {
        using underlying = typename std::underlying_type<EnumType>::type;
        return static_cast<underlying>(val);
    }

    template <typename EnumType, typename IntType>
    constexpr EnumType as_enum(IntType val) {
        static_assert(std::is_enum_v<EnumType>
                          && (std::is_same_v<IntType, std::underlying_type_t<EnumType>>
                              || std::numeric_limits<IntType>::max()
                                     <= std::numeric_limits<std::underlying_type_t<EnumType>>::max()),
                      "the integer type must be equal to or more narrow than the converted to enum's underlying "
                      "ordinal type");

        return static_cast<EnumType>(val);
    }

    template <typename EnumType, class IntType, std::enable_if_t<std::is_integral_v<IntType>, int> = 0>
    constexpr EnumType operator<<(
        const EnumType Arg,
        const IntType Shift) noexcept { // bitwise LEFT SHIFT, every static_cast is intentionals
        using underlying = typename std::underlying_type<EnumType>::type;
        return static_cast<EnumType>(static_cast<underlying>(static_cast<underlying>(Arg) << Shift));
    }

    template <typename EnumType, class IntType, std::enable_if_t<std::is_integral_v<IntType>, int> = 0>
    constexpr EnumType operator>>(
        const EnumType Arg,
        const IntType Shift) noexcept { // bitwise RIGHT SHIFT, every static_cast is intentional
        using underlying = typename std::underlying_type<EnumType>::type;
        return static_cast<EnumType>(static_cast<underlying>(static_cast<underlying>(Arg) >> Shift));
    }

    template <typename EnumType, std::enable_if_t<std::is_enum_v<EnumType>, int> = 0>
    constexpr EnumType operator|(const EnumType Left,
                                 const EnumType Right) noexcept { // bitwise OR, every static_cast is intentional
        using underlying = typename std::underlying_type<EnumType>::type;
        return static_cast<EnumType>(
            static_cast<EnumType>(static_cast<underlying>(Left) | static_cast<underlying>(Right)));
    }

    template <typename EnumType, class IntType, std::enable_if_t<std::is_integral_v<IntType>, int> = 0>
    constexpr EnumType operator|(const EnumType Left,
                                 const IntType Right) noexcept { // bitwise OR, every static_cast is intentional
        using underlying = typename std::underlying_type<EnumType>::type;
        return static_cast<EnumType>(
            static_cast<EnumType>(static_cast<underlying>(Left) | static_cast<underlying>(Right)));
    }

    template <typename EnumType, std::enable_if_t<std::is_enum_v<EnumType>, int> = 0>
    constexpr EnumType operator&(const EnumType Left,
                                 const EnumType Right) noexcept { // bitwise AND, every static_cast is intentional
        using underlying = typename std::underlying_type<EnumType>::type;
        return static_cast<EnumType>(
            static_cast<underlying>(static_cast<underlying>(Left) & static_cast<underlying>(Right)));
    }

    template <typename EnumType, class IntType, std::enable_if_t<std::is_integral_v<IntType>, int> = 0>
    constexpr EnumType operator&(const EnumType Left,
                                 const IntType Right) noexcept { // bitwise OR, every static_cast is intentional
        using underlying = typename std::underlying_type<EnumType>::type;
        return static_cast<EnumType>(
            static_cast<underlying>(static_cast<underlying>(Left) & static_cast<underlying>(Right)));
    }

    template <typename EnumType, std::enable_if_t<std::is_enum_v<EnumType>, int> = 0>
    constexpr EnumType operator^(const EnumType Left,
                                 const EnumType Right) noexcept { // bitwise XOR, every static_cast is intentional
        using underlying = typename std::underlying_type<EnumType>::type;
        return static_cast<EnumType>(
            static_cast<underlying>(static_cast<underlying>(Left) ^ static_cast<underlying>(Right)));
    }

    template <typename EnumType, class IntType, std::enable_if_t<std::is_integral_v<IntType>, int> = 0>
    constexpr EnumType operator^(const EnumType Left,
                                 const IntType Right) noexcept { // bitwise OR, every static_cast is intentional
        using underlying = typename std::underlying_type<EnumType>::type;
        return static_cast<EnumType>(
            static_cast<underlying>(static_cast<underlying>(Left) ^ static_cast<underlying>(Right)));
    }

    template <typename EnumType, std::enable_if_t<std::is_enum_v<EnumType>, int> = 0>
    constexpr EnumType operator~(const EnumType Arg) noexcept { // bitwise NOT, every static_cast is intentional
        using underlying = typename std::underlying_type<EnumType>::type;
        return static_cast<EnumType>(static_cast<underlying>(~static_cast<underlying>(Arg)));
    }

    template <typename EnumType, class IntType, std::enable_if_t<std::is_integral_v<IntType>, int> = 0>
    constexpr EnumType& operator<<=(EnumType& Arg, const IntType Shift) noexcept { // bitwise LEFT SHIFT
        return Arg = Arg << Shift;
    }

    template <typename EnumType, class IntType, std::enable_if_t<std::is_integral_v<IntType>, int> = 0>
    constexpr EnumType& operator>>=(EnumType& Arg, const IntType Shift) noexcept { // bitwise RIGHT SHIFT
        return Arg = Arg >> Shift;
    }

    template <typename EnumType, std::enable_if_t<std::is_enum_v<EnumType>, int> = 0>
    constexpr EnumType& operator|=(EnumType& Left, const EnumType Right) noexcept { // bitwise OR
        return Left = Left | Right;
    }

    template <typename EnumType, std::enable_if_t<std::is_enum_v<EnumType>, int> = 0>
    constexpr EnumType& operator&=(EnumType& Left, const EnumType Right) noexcept { // bitwise AND
        return Left = Left & Right;
    }

    template <typename EnumType, std::enable_if_t<std::is_enum_v<EnumType>, int> = 0>
    constexpr EnumType& operator^=(EnumType& Left, const EnumType Right) noexcept { // bitwise XOR
        return Left = Left ^ Right;
    }

    template <typename FromEnumType,
              typename ToEnumType,
              std::enable_if_t<std::is_enum_v<FromEnumType> && std::is_enum_v<ToEnumType>, int> = 0>
    ToEnumType convert_enum(FromEnumType from) {
        using underlying_from = std::underlying_type_t<FromEnumType>;
        using underlying_to = std::underlying_type_t<ToEnumType>;

        underlying_from from_int = as_integer(from);
        if (std::numeric_limits<underlying_from>::max() >= std::numeric_limits<underlying_from>::max()
            && from_int <= std::numeric_limits<underlying_to>::max()) {
            return static_cast<ToEnumType>(static_cast<underlying_to>(from_int));
        }
    }

    template <typename EnumType, EnumType... Values>
    class EnumCheck;

    template <typename EnumType>
    class EnumCheck<EnumType> {
    public:
        template <typename IntType>
        static bool constexpr is_value(IntType) {
            return false;
        }
    };

    template <typename EnumType, EnumType V, EnumType... Next>
    class EnumCheck<EnumType, V, Next...> : private EnumCheck<EnumType, Next...> {
        using super = EnumCheck<EnumType, Next...>;

    public:
        template <typename IntType>
        static bool constexpr is_value(IntType v) {
            return v == static_cast<IntType>(V) || super::is_value(v);
        }
    };

} // namespace nog
