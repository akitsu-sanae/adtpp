/*============================================================================
  Copyright (C) 2017 akitsu sanae
  https://github.com/akitsu-sanae/adtpp
  Distributed under the Boost Software License, Version 1.0. (See accompanying
  file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
============================================================================*/

#ifndef ADT_PP_HPP
#define ADT_PP_HPP

#include <string>
#include <vector>
#include <utility>

struct Adt {
    using element_t = std::pair<std::string, std::string>;
    std::string struct_name;
    std::vector<element_t> elements;

    explicit Adt(std::string const& name) :
        struct_name(name)
    {}

    Adt& add(element_t&& e) {
        elements.push_back(std::move(e));
        return *this;
    }
};

std::ostream& operator<<(std::ostream&, Adt const&);

#endif
