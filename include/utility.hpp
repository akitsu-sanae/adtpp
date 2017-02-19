/*============================================================================
  Copyright (C) 2017 akitsu sanae
  https://github.com/akitsu-sanae/adtpp
  Distributed under the Boost Software License, Version 1.0. (See accompanying
  file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
============================================================================*/

#ifndef ADT_PP_UTILITY_HPP
#define ADT_PP_UTILITY_HPP

#include <sstream>

struct format_too_many_args {};

inline static std::string format(std::string const& str) {
    return str;
}

template<typename Arg, typename ... Args>
inline static std::string format(std::string const& str, Arg&& arg, Args&& ... args) {
    auto pos = str.find("{}");
    if (pos == std::string::npos)
        throw format_too_many_args{};
    std::string after = str.substr(pos+2, str.length());
    std::stringstream ss;
    ss << std::forward<Arg>(arg);
    return str.substr(0, pos) + ss.str() + format(after, args ...);

}

#endif

