#pragma once
#include <cstdint>
#include <format>
#include <string>
#include <type_traits>

namespace nog {

    template <typename T>
    concept IsErrKindEnum = requires {
        requires std::is_enum_v<T>;
        requires std::is_same_v<std::underlying_type_t<T>, uint32_t>;
        requires requires(const T& err_kind) { error_msg(err_kind); };
        requires requires { T::None; };
    };

    template <IsErrKindEnum T>
    class Error {
    private:
        T ec;
        std::string detail;

    public:
        Error(const T& ec) : ec{ec} {}
        Error(const T& ec, const std::string& detail) : ec{ec}, detail{detail} {}

        std::string msg() const {
            if (!this->detail.empty()) {
                return std::format("{}; Details: {}", error_msg(this->ec), this->detail);
            }

            return error_msg(ec);
        }

        uint32_t error_code() const { return uint32_t(this->ec); }
        inline static Error<T> none() { return Error<T>(T::None); }

        operator bool() const { return this->ec == T::None; }

        friend std::ostream& operator<<(std::ostream& stream, const Error<T>& this_guy) {
            return (stream << this_guy.msg());
        }
    };

} // namespace nog
