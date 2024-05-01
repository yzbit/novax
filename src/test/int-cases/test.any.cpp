#include <any>
#include <functional>
#include <stdio.h>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

template <typename... Args>
void create2( const char* name_, Args&&... par_ ) {
    create( std::forward<Args>( par_ )... );
    // create( std::forward<Args>( par_ )... );
}

#if 0
template <>
int createx() {
    create( std::get<0>( t ), create::get<1>( t ) );
    return 0;
}
#endif

template <typename... Ts>
void forward( Ts&&... ts ) {
    create( std::forward<Ts>( ts )... );
}

template <typename... Args>
struct pars_t {
    using val_t = std::tuple<Args...>;

    pars_t( Args... args )
        : _val( std::forward<Args>( args )... ) {}

    val_t& values() { return _val; }

private:
    val_t _val;
};

template <typename T>
struct binding_t {
    binding_t( const T& v_ )
        : v( v_ ) {}

    binding_t& operator==( const T& v_ ) {
        v = v_;
    }

    T v;
};

template <typename... Ts>
struct arg_t {
    arg_t( Ts... ts )
        : _tuple( ts... ) {}

    auto& get( std::size_t i ) {
        return std::get<0>( _tuple );
    }

    static constexpr size_t size = std::tuple_size_v<std::tuple<Ts...>>;

    template <typename F>
    void for_each( F f ) {
        for_each_impl( f, std::make_index_sequence<sizeof...( Ts )>{} );
    }

private:
    template <typename F, size_t... N>
    void for_each_impl( F f, std::index_sequence<N...> ) {
        ( std::invoke( f, std::get<N>( _tuple ) ), ... );
    }

    std::tuple<Ts...> _tuple;
};

#define DEFAULT_ARG_VALUES( ... )    \
    auto args() {                    \
        return arg_t{ __VA_ARGS__ }; \
    };

#include <string>
#include <vector>
using stringlist_t = std::vector<std::string>;
#define DECLARE_ARG_NAMES( ... )                \
    stringlist_t& arg_names() {                 \
        static stringlist_t __v{ __VA_ARGS__ }; \
        return __v;                             \
    }

/// MyTuple t{1, "hello"}; // 自动推断为MyTuple<int, const char*>

DEFAULT_ARG_VALUES( 1, 2.0 );
DECLARE_ARG_NAMES( "length", "上轨" )

void create( int p1, double f ) {
    printf( "get pars:%d %lf\n", p1, .0 );
}

template <typename T, std::size_t... Is>
void call_create( T&& t, std::index_sequence<Is...> ) {
    create( t.get( Is )... );
}

// 因为不知道tuple的具体类型,所以必须是模板函数
template <typename T>
void call_create( T&& t ) {
    call_create( std::forward<T>( t ), std::make_index_sequence<std::decay_t<T>::size>{} );
}
//
int main() {
    auto tt = args();
    tt.for_each( []( auto& e ) {
        printf( "aaa\n" );
    } );

    call_create( tt );
    // std::tuple_size_v<decltype( tt )>;

    for ( auto& s : arg_names() ) {
        printf( "arg name=%s\n", s.c_str() );
    }

#if 0
    // DECLARE_ARGS3( ttt, 1, 2, 3 );

    auto aa = args();
    // for ( auto& e : t ) {
    // }

    binding_t b1 = 1;
    binding_t b2 = 5.0;

    // arg_t a={1,2,3};

    // DEFAULT_ARGS( abc, 1, 2.0, 1 );

    // DECLARE_ARGS( int, double )
    // tple = { 1, 2.0 };

    /// DEFAULT_ARGS2( xyz, 1, 2, 3 );

    printf( "b1.v=%d, b2.v=%lf\n", b1.v, b2.v );

    // pars_t p0 = { b1.v, b2.v };
    std::tuple p0 = { b1.v, b2.v };
    call_create( p0 );

    // 甚至可能连call_create(p)都不需要
    // call_create( b1, b2 );  // 自动forward给create就好

    //<2>
    std::tuple p = { 1, 2.0 };
    call_create( p );

    //[1]
    std::tuple<int, double> t = { 1, 3.0 };
    call_create( t );
    // std::any p1 = 1;
    // std::any p2 = 2.0;
    // using par_t = std::variant<int, double>;
    //    par_t p1    = 1;
    //    par_t p2    = 2.0;
    int    p1 = 1;
    double p2 = 2.0;

    // create2( "a creation", std::get<0>( t ), std::get<1>( t ) );
#endif
    return 0;
}