#ifndef A37995CF_4ECF_464C_B471_04E26CC7055C
#define A37995CF_4ECF_464C_B471_04E26CC7055C
#include <functional>
#include <stdexcept>
#include <string.h>

#include "ns.h"

CUB_NS_BEGIN

struct Series final {
    struct element_t {
        union {
            int    i;
            double f;
            void*  p;
            char   arr[ 8 ];
        };

        element_t() = default;
        element_t( const element_t& e_ );
        element_t( int i_ );
        element_t( double f_ );
        element_t( const char a_[] );
        element_t( void* p );
        element_t& operator=( void* p );
        element_t& operator=( const char a_[] );
        element_t& operator=( int i_ );
        element_t& operator=( double f_ );

        operator void*();
        operator int() const;
        operator double() const;
        operator char*();
        operator const char*() const;
    };

    using free_t = std::function<void( void* )>;

    Series( int n_, free_t free_ = default_free() );
    ~Series();
    static free_t default_free();

    void init( std::function<void( element_t& e_ )> );
    //指针类型会有赋值困难,默认数组是已经完整的,只shift即可
    // void append( const element_t& t_ );
    void shift();
    int  size();
    void for_each( std::function<bool( element_t& e_ )> op_ );
    void update( int index_, const element_t& t_ );

    element_t& operator[]( int index_ );
    element_t& get( int index_ );
    element_t& at( int index_ );

private:
    bool is_valid_slot( int slot_ );
    // 0 是最右边的那个,也就是_end--把K线序列映射为存储序列
    int map( int index_ );

private:
    free_t     _free = nullptr;
    int        _total;
    int        _begin = 0;
    int        _end   = 0;
    element_t* _values;
};

inline Series::element_t::element_t( void* p_ )
    : p( p_ ) {
}

inline Series::element_t& Series::element_t::operator=( void* p_ ) {
    p = p_;
}

inline Series::element_t::element_t( const element_t& e_ ) {
    memcpy( arr, e_.arr, sizeof( arr ) );
}

inline Series::element_t::element_t( int i_ )
    : i( i_ ) {}

inline Series::element_t::element_t( double f_ )
    : f( f_ ) {}

inline Series::element_t::element_t( const char a_[] ) {
    memcpy( arr, a_, sizeof( arr ) );
}

inline Series::element_t& Series::element_t::operator=( const char a_[] ) {
    memcpy( arr, a_, sizeof( arr ) );
    return *this;
}

inline Series::element_t& Series::element_t::operator=( int i_ ) {
    i = i_;
    return *this;
}

inline Series::element_t& Series::element_t::operator=( double f_ ) {
    f = f_;
    return *this;
}

inline Series::element_t::operator void*() {
    return p;
}

inline Series::element_t::operator int() const {
    return i;
}

inline Series::element_t::operator double() const {
    return f;
}

inline Series::element_t::operator char*() {
    return arr;
}

inline Series::element_t::operator const char*() const {
    return arr;
}

#define ROUND_PTR( _index_, _capa_sz_ ) _index_ = _index_ == _capa_sz_ \
                                                      ? 0              \
                                                      : _index_
inline Series::Series( int n_, free_t free_ )
    : _total( n_ + 1 )
    , _free{ free_ } {
    _values = new element_t[ _total ]{ 0 };

    shift();
}

inline Series::~Series() {
    for_each( [ & ]( auto& e_ ) {
        _free( e_.p );
        return true;
    } );

    delete[] _values;
}

inline Series::free_t Series::default_free() {
    static const auto default_free = []( void* ) {};
    return default_free;
}

inline void Series::init( std::function<void( element_t& e_ )> f_ ) {
    for ( int i = 0; i < _total; ++i )
        f_( _values[ i ] );
}

inline void Series::shift() {
    ++_end;
    ROUND_PTR( _end, _total );

    if ( _end == _begin ) {
        ++_begin;
        ROUND_PTR( _begin, _total );
    }
}

/*
inline void Series::append( const element_t& t_ ) {
    _values[ _end ] = t_;

    ++_end;
    ROUND_PTR( _end, _total );

    if ( _end == _begin ) {
        _free( _values[ _begin ].p );

        ++_begin;
        ROUND_PTR( _begin, _total );
    }
}
*/

inline int Series::size() {
    return _end >= _begin
               ? _end - _begin
               : _end + _total - _begin;  //-减少处罚运算
}

inline void Series::for_each( std::function<bool( element_t& e_ )> op_ ) {
    for ( int loop = _begin; loop != _end; loop = ( loop + 1 ) % _total ) {
        if ( !op_( _values[ loop ] ) ) break;
    }
}

inline void Series::update( int index_, const element_t& t_ ) {
    _values[ map( index_ ) ] = t_;
}

inline bool Series::is_valid_slot( int slot_ ) {
    if ( 0 == size() ) return false;

    auto end = _end;
    if ( _begin > _end ) {
        end += _total;
        slot_ += _total;
    }

    return slot_ >= _begin && slot_ < end;
}

inline Series::element_t& Series::operator[]( int index_ ) {
    return get( index_ );
}

inline Series::element_t& Series::get( int index_ ) {
    return _values[ map( index_ ) ];
}

inline Series::element_t& Series::at( int index_ ) {
    return get( index_ );
}

inline int Series::map( int index_ ) {
    if ( index_ >= size() ) {
        throw std::range_error( "series index overflow" );
    }

    int index = _end - index_ - 1;
    return index < 0 ? index + _total : index;
}

CUB_NS_END
#endif /* A37995CF_4ECF_464C_B471_04E26CC7055C */
