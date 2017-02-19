# adt++

`adt++` is the tool to use ADT (Algebraic Data Type) in C++.  
This tool read a text file, and export C++ header file which defines ADT class.  

note: `adt++` requires C++14 to compile generated header.

# Install
```
$ git clone https://github.com/akitsu-sanae/adt++
$ make
```
then, you will find `./build/adt++`.

# How to use

### Generate C++ Header file

`adt++` imports a text file, and exports C++ header file.
Here is `examples/json.adt`:
```
Json {
    Null of std::nullptr_t
    Boolean of bool
    Number of double
    String of std::basic_string<char>
    Array of std::vector<Json>
    Object of std::map<std::string, Json>
}
```
run `./build/adt++ examples/json.adt > result.hpp`  
or `cat examples/json.adt | ./build/adt++ > result.hpp`

### Use generated header file

See `examples/how_to_use.cpp`

```cpp
#include <iostream>
#include "../result.hpp"

int main() {
    Json json{12.0};
    std::cout << *Json::match<double>{json}
        .when_null([](std::nullptr_t) { return 42.0; })
        .when_number([](double n) { return n * 2; })
        .otherwise([]() { return 1.0; }) << std::endl;
}
```

# Copyright
Copyright (C) 2017 akitsu sanae.  
Distributed under the Boost Software License, Version 1.0. 
(See accompanying file LICENSE or copy at http://www.boost/org/LICENSE_1_0.txt)  


