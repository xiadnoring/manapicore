// modules/test.cppm
module;
#include <string>

export module test;

export int add(int a, int b) {
    return a + b;
}

export class Greeter {
public:
    explicit Greeter(std::string name) : name_(std::move(name)) {}
    std::string greet() const { return "Hello, " + name_ + "!"; }
private:
    std::string name_;
};

export template <typename T>
T twice(T value) { return value + value; }

export inline constexpr int MAGIC = 42;