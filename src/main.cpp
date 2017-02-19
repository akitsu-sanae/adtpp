/*============================================================================
  Copyright (C) 2017 akitsu sanae
  https://github.com/akitsu-sanae/adtpp
  Distributed under the Boost Software License, Version 1.0. (See accompanying
  file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
============================================================================*/

#include <iostream>
#include <fstream>
#include <cctype>

#include "adt.hpp"
#include "utility.hpp"

void trim(std::string& input) {
    auto start = std::begin(input);
    while (std::isspace(*start))
        start++;
    auto last = std::end(input);
    while (std::isspace(*(last-1)))
        last--;
    input = std::string{start, last};
}

Adt parse(std::istream& input) {
    std::string buf;
    input >> buf;
    Adt result{buf};
    input >> buf;
    if (buf != "{")
        throw std::logic_error{"parse error: expected '{'"};
    while (true) {
        std::string label;
        std::string type;
        input >> label;
        if (label == "}")
            break;
        input >> buf;
        if (buf != "of")
            throw std::logic_error{"parse error: expected 'of'"};
        std::getline(input, type);
        trim(type);
        result.add(std::make_pair(label, type));
    }
    return result;
}

int main(int argc, const char* argv[]) {
    if (argc == 1) {
        std::cout << parse(std::cin) << std::endl;
    } else if (argc == 2) {
        std::ifstream input{argv[1]};
        std::cout << parse(input) << std::endl;
    } else {
        std::cerr << format("Usage: {} [filename]", argv[0]) << std::endl;
        std::cerr<< "if filename is not given, read from stdin" << std::endl;
        return -1;
    }
}

