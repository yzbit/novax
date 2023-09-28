#ifndef E93F5C75_9223_409D_8F98_DFFDE2E179BF
#define E93F5C75_9223_409D_8F98_DFFDE2E179BF
#include <any>
#include <array>
#include <cub.h>
#include <functional>
#include <optional>

CUB_NS_BEGIN

template <typename T, int N>
struct Series {
    void add( const T& t_ ) {
        _values[ _end ] = t_;

        int update_end = ( _end + 1 ) % N;
        if ( update_end == _begin ) {
            _begin = ( _begin + 1 ) % N;
        }

        _end = update_end;
    }

    int size() {
        return ( _end + N - _begin ) % N;
    }

    void for_each( std::function<bool( const T& )> op_ ) {
        for ( int loop = _begin; loop != _end; loop = ( loop + 1 ) % N ) {
            if ( !op_( _values[ loop ] ) ) break;
        }
    }

    int update( int slot_, const T& t_ ) {
        if ( !is_valid_slot( slot_ ) ) return -1;

        _values[ slot_ ] = t_;
    }

    // not T&
    std::optional<T> get( int slot_ ) {
        if ( !is_valid_slot() )
            return std::nullopt;

        return _values[ slot_ ];
    }

private:
    bool is_valid_slot( int slot_ ) {
        if ( 0 == size() ) return false;

        auto end = _end;
        if ( _begin > _end ) {
            end += N;
            slot_ += N;
        }
        return slot_ >= _begin && slot_ < end;
    }

private:
    int              _begin = 0;
    int              _end   = 0;
    std::array<T, N> _values;
};

struct AnyType {
};

struct AlgoContext {
};
/*
可能有多组只,每组值都是一个series但是类型可能不一样
要不要一个基准指标,比如5分钟,10分钟, 像ui一样, 没有基准的话,如何知道当前是第几根呢
....
*/
struct Algo {
    Algo() {
        initialize();
    }

    virtual ~Algo() {}
    virtual void on_calculate( int current_bar_, AlgoContext* ctx_ ){};

    int add_series( int type_, int size_ );
    int series_cnt();
    // SeriesValue& value( int track_, int index_ );
    // void set_value( int track_, int index_, const SeriesValue& sv_ );
private:
    void initialize() {
       // for ( int i = 0; i <= total; ++i ) {
       //     on_calculate( i, context );
       // }
    }
};

struct AlgoArg {
    std::any value;

    operator int() const {
        return std::any_cast<int>( value );
    }

    operator const char*() const {
        return std::any_cast<const char*>( value );
    }

    operator std::string() const {
        return std::any_cast<std::string>( value );
    }
};

static constexpr int kMaxArgsSupport = 16;

using ArgPack = std::array<AlgoArg, kMaxArgsSupport>;

CUB_NS_END
#endif /* E93F5C75_9223_409D_8F98_DFFDE2E179BF */
