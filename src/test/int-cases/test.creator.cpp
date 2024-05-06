/************************************************************************************
The MIT License

Copyright (c) 2024 YaoZinan  [zinan@outlook.com, nvx-quant.com]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

* \author: yaozn(zinan@outlook.com)
* \date: 2024
**********************************************************************************/

#if 0
#include <tuple>
#include <utility>

std::tuple _tuple = { 1 };

struct Ma {
    static Ma* create( int ) {
        return nullptr;
    }
};

struct Mb {
    static Mb* create( int ) {
        return nullptr;
    }
};

int main() {
    auto t = std::make_tuple( &Ma::create );
    // std::swap( _tuple, t );

    std::tuple_cat( _tuple, t );

    t = make_tuple( 1 );

    // std::swap( tt, _tuple );

    return 0;
}

#endif
#include <functional>
#include <iostream>
#include <map>
#include <string>

// 定义一个通用的调用函数，它接受一个std::function和一个参数包
template <typename Func, typename... Args>
void callFunction( Func&& f, Args&&... args ) {
    f( std::forward<Args>( args )... );
}

int main() {
    std::map<std::string, std::function<void()>> functionMap;

    // 使用Lambda将不同签名的函数适配为std::function<void()>
    functionMap[ "func1" ] = []( int a ) { std::cout << "func1 with int: " << a << std::endl; };
    functionMap[ "func2" ] = []( double a, double b ) { std::cout << "func2 with doubles: " << a << ", " << b << std::endl; };
    functionMap[ "func3" ] = []( const std::string& a, int b ) { std::cout << "func3 with string and int: " << a << ", " << b << std::endl; };

    // 调用函数
    callFunction( functionMap[ "func1" ], 10 );           // func1 with int: 10
    callFunction( functionMap[ "func2" ], 1.5, 2.5 );     // func2 with doubles: 1.5, 2.5
    callFunction( functionMap[ "func3" ], "hello", 30 );  // func3 with string and int: hello, 30

    return 0;
}