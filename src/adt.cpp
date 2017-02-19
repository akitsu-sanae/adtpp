/*============================================================================
  Copyright (C) 2017 akitsu sanae
  https://github.com/akitsu-sanae/adtpp
  Distributed under the Boost Software License, Version 1.0. (See accompanying
  file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
============================================================================*/

#include <iostream>
#include <algorithm>
#include "adt.hpp"
#include "utility.hpp"

std::string convert_to_label(std::string const& str) {
    std::string result = str;
    std::transform(std::begin(result), std::end(result), std::begin(result), tolower);
    return result;
}

std::string convert_to_dtor_name(std::string str) {
    // remove template parameter
    // ex) std::vector<double> ==> std::vector
    while (true) {
        int temp_count = 0;
        auto temp_start = std::string::npos;
        auto temp_end = std::string::npos;
        for (auto it=std::begin(str); it!=std::end(str); it++) {
            if (*it == '<') {
                if (temp_count == 0)
                    temp_start = it - std::begin(str);
                temp_count++;
            }
            if (*it == '>') {
                temp_count--;
                if (temp_count == 0)
                    temp_end = it - std::begin(str);
            }
        }
        if (temp_start == std::string::npos)
            break;
        if (temp_end == std::string::npos)
                break;
        str.erase(temp_start, temp_end);
    }

    // remove namespace
    // ex) std::vector ==> vector
    auto found = str.find_last_of(':');
    if (found != std::string::npos)
        str.erase(0, found+1);
    return str;
}
std::ostream& operator<<(std::ostream& os, Adt const& adt) {
    os << format("#ifndef ADT_{}_HPP", adt.struct_name) << std::endl;
    os << format("#define ADT_{}_HPP", adt.struct_name) << std::endl;

    os << std::endl;
    os << "#include <vector>" << std::endl;
    os << "#include <map>" << std::endl;
    os << "#include <string>" << std::endl;
    os << "#include <memory>" << std::endl;
    os << "// this file is generated from adt++ (https://github.com/akitsu-sanae/adt++)" << std::endl;
    os << std::endl;

    os << format("struct {} {", adt.struct_name) << std::endl;

    // types
    os << "    enum class Type {" << std::endl;
    for (auto const& e : adt.elements)
        os << format("        {},", e.first) << std::endl;
    os << "    };" << std::endl;

    // ctors
    for (auto const& e : adt.elements) {
        os << format("    explicit {}({} const& v) :", adt.struct_name, e.second) << std::endl;
        os << format("        m_type(Type::{})", e.first) << std::endl;
        os << "    {" << std::endl;
        os << "        assign(v);" << std::endl;
        os << "    }" << std::endl;
    }


    // copy ctor
    os << format("    {}({} const& rhs) :", adt.struct_name, adt.struct_name) << std::endl;
    os << "        m_type(rhs.m_type)" << std::endl;
    os << "    {" << std::endl;
    os << "        switch (m_type) {" << std::endl;
    for (auto const& e : adt.elements) {
        os << format("        case Type::{}:", e.first) << std::endl;
        os << format("            assign(rhs.{});", convert_to_label(e.first)) << std::endl;
        os << "            break;" << std::endl;
    }
    os << "        }" << std::endl;
    os << "    }" << std::endl;

    // move ctor
    os << format("    {}({}&& rhs) :", adt.struct_name, adt.struct_name) << std::endl;
    os << "        m_type(rhs.m_type)" << std::endl;
    os << "    {" << std::endl;
    os << "        switch (m_type) {" << std::endl;
    for (auto const& e : adt.elements) {
        os << format("        case Type::{}:", e.first) << std::endl;
        os << format("            assign(std::move(rhs.{}));", convert_to_label(e.first)) << std::endl;
        os << "            break;" << std::endl;
    }
    os << "        }" << std::endl;
    os << "    }" << std::endl;

    os << "    Type type() const { return m_type; }" << std::endl;

    // match
    os << "    template<typename T>" << std::endl;
    os << "    struct match {" << std::endl;
    os << format("    {} const& adt;", adt.struct_name) << std::endl;
    os << "        std::unique_ptr<T> result = nullptr;" << std::endl;
    os << std::endl;
    os << "        using result_t = T; " << std::endl;
    os << std::endl;
    os << format("        explicit match({} const& adt) :", adt.struct_name);
    os << "            adt(adt)" << std::endl;
    os << "        {}" << std::endl;

    for (auto const& e : adt.elements) {
        os << "        template<typename F>" << std::endl;
        os << format("        match& when_{}(F const& f) {", convert_to_label(e.first)) << std::endl;
        os << format("            if (adt.type() == Type::{} && !result)", e.first) << std::endl;
        os << format("                result = std::make_unique<T>(f(adt.{}));", convert_to_label(e.first)) << std::endl;
        os << "            return *this;" << std::endl;
        os << "        }" << std::endl;
    }

    os << "        template<typename F>" << std::endl;
    os << "        match& otherwise(F const& f) {" << std::endl;
    os << "            if (!result)" << std::endl;
    os << "                result = std::make_unique<T>(f());" << std::endl;
    os << "            return *this;" << std::endl;
    os << "        }" << std::endl;

    os << "        struct not_satisfy{};" << std::endl;

    os << "        T operator*() {" << std::endl;
    os << "            if (!result)" << std::endl;
    os << "                throw not_satisfy{};" << std::endl;
    os << "            return *result;" << std::endl;
    os << "        }" << std::endl;

    // end match struct
    os << "    };" << std::endl;

    // dtor
    os << format("    ~{}() {", adt.struct_name) << std::endl;
    os << "        clear();" << std::endl;
    os << "    }" << std::endl;

    // as

    os << "private:" << std::endl;

    os << "    union {" << std::endl;
    for (auto const& e : adt.elements) {
        auto label = convert_to_label(e.first);
        os << format("        {} {};", e.second, label) << std::endl;
    }
    os << "    };" << std::endl;


    // assign
    for (auto const& e : adt.elements) {
        // copy
        os << format("    void assign({} const& src) {", e.second) << std::endl;
        os << format("        new (&{}) {}{src};", convert_to_label(e.first), e.second) << std::endl;
        os << "    }" << std::endl;

        // move
        os << format("    void assign({}&& src) {", e.second) << std::endl;
        os << format("        new (&{}) {}{src};", convert_to_label(e.first), e.second) << std::endl;
        os << "    }" << std::endl;

    }

    // clear
    os << "    void clear() {" << std::endl;
    os << "        switch (m_type) {" << std::endl;
    for (auto const& e : adt.elements) {
        os << format("        case Type::{}:", e.first) << std::endl;
        auto label = convert_to_label(e.first);
        auto type = convert_to_dtor_name(e.second);
        std::vector<std::string> primitives = {
            "int", "double", "nullptr_t", "bool", "size_t", "char"
        };
        auto found = std::find(std::begin(primitives), std::end(primitives), type);
        if (found == std::end(primitives))
            os << format("            {}.~{}();", label, type) << std::endl;
        os << "            break;" << std::endl;
    }
    os << "        }" << std::endl;
    os << "    }" << std::endl;


    os << "    Type m_type;" << std::endl;

    // end struct
    os << "};" << std::endl;

    os << "#endif" << std::endl;
    return os;
}

