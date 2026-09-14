#pragma once
#include "nog/util/types.h"

#include <atomic>
#include <functional>
#include <memory>
#include <stdexcept>

namespace nog {

    template <IsIntegerType IntType>
    class AutoIncId {
    protected:
        static std::atomic<IntType> ID_ROOT;
        IntType val;

    public:
        inline static AutoIncId INVALID{std::numeric_limits<IntType>::max()};

        AutoIncId(IntType value) : val{value} {}
        virtual ~AutoIncId() = default;

        IntType value() const { return this->val; }
        AutoIncId next() const { return ID_ROOT.fetch_add(1, std::memory_order::relaxed); }
        bool is_valid() const { return this->val == std::numeric_limits<IntType>::max(); }
        bool is_invalid() const { return !this->is_valid(); }
        auto operator<=>(const AutoIncId& other) const = default;
    };

    template <IsIntegerType IntType>
    class PredicateId;

    template <IsIntegerType IntType>
    using PredicateFnTy = std::function<PredicateId<IntType>(const IntType&)>;

    template <IsIntegerType IntType>
    class PredicateId {
    protected:
        static PredicateFnTy<IntType> next_id_fn;
        IntType val;

    public:
        inline static PredicateId INVALID{std::numeric_limits<IntType>::max()};

        inline static void set_predicate_fn(PredicateFnTy<IntType> fn) {
            if (!bool(next_id_fn))
                next_id_fn = fn;
        }

        PredicateId(IntType value) : val{value} {}

        virtual ~PredicateId() = default;

        IntType value() const { return this->val; }

        PredicateId next() const {
            if (PredicateId::next_id_fn) {
                return next_id_fn(this->val);
            }

            throw std::runtime_error(
                "predicate function for PredicateId has not been set. Cannot generate next id value");
        }

        bool is_valid() const { return this->val == std::numeric_limits<IntType>::max(); }
        bool is_invalid() const { return !this->is_valid(); }

        auto operator<=>(const PredicateId& other) const = default;
    };

    // This type allows for putting the two different Id types into a single list or for a function to accept
    // either type of Id as long as they contain the same inner integer type
    template <IsIntegerType IntType, typename T>
    class IdInterface {
        friend class AutoIncId<IntType>;
        friend class PredicateId<IntType>;

    private:
        template <typename InnerT>
        struct InterfaceTy {
            virtual ~InterfaceTy() = default;
            virtual IntType value_impl() const = 0;
            virtual InnerT next_impl() const = 0;
            virtual bool is_valid_impl() const = 0;
            virtual bool is_invalid_impl() const = 0;
        };

        template <typename InnerT>
        struct Model : public InterfaceTy<IntType> {
            Model(T value) : data(std::move(value)) {}
            virtual IntType value_impl() const override { return data.value(); }
            virtual InnerT next_impl() const override { return data.next(); }
            virtual bool is_valid_impl() const override { return data.is_valid(); }
            virtual bool is_invalid_impl() const override { return data.is_invalid(); }

            InnerT data;
        };

        std::unique_ptr<InterfaceTy<T>> self;

    public:
        IdInterface(T obj) : self(std::make_unique<Model<T>>(std::move(obj))) {}

        IntType value() const { return self.value_impl(); }
        T next() const { return self.next_impl(); }
        bool is_valid() const { return self.is_valid_impl(); }
        bool is_invalid() const { return self.is_invalid_impl(); }
    };
} // namespace nog
