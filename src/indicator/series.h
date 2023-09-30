#ifndef A37995CF_4ECF_464C_B471_04E26CC7055C
#define A37995CF_4ECF_464C_B471_04E26CC7055C

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
#endif /* A37995CF_4ECF_464C_B471_04E26CC7055C */
