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