
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// 1. The Wrapper Type (The user only interacts with this non-templated class)
class Speaker {
public:
    // Templated constructor accepts ANY type that has a 'speak()' method
    template <typename T>
    Speaker(T obj) : self(std::make_unique<Model<T>>(std::move(obj))) {}

    // Uniform public interface
    void speak() const { self->speak_impl(); }

private:
    // 2. The Internal Concept
    struct Concept {
        virtual ~Concept() = default;
        virtual void speak_impl() const = 0;
    };

    // 3. The Templated Model
    template <typename T>
    struct Model : public Concept {
        Model(T value) : data(std::move(value)) {}
        void speak_impl() const override { data.speak(); } // Forwards the call
        T data;
    };

    std::unique_ptr<Concept> self;
};

// Two completely unrelated classes with NO shared base class
struct Dog {
    void speak() const { std::cout << "Woof!\n"; }
};
struct Robot {
    void speak() const { std::cout << "Beep Boop!\n"; }
};

int main() {
    // We can store completely different types in a uniform vector!
    std::vector<Speaker> speakers;
    speakers.push_back(Dog{});
    speakers.push_back(Robot{});

    for (const auto& speaker : speakers) {
        speaker.speak();
    }
}
