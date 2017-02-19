#include <iostream>

// assume that generated filename is result.hpp
#include "../result.hpp"

int main() {
    Json json{12.0};
    std::cout << *Json::match<double>{json}
        .when_null([](std::nullptr_t) { return 42.0; })
        .when_number([](double n) { return n * 2; })
        .otherwise([]() { return 1.0; }) << std::endl;
}

