#ifndef EBC2EDD7_C2AC_4519_9357_CE387B50513F
#define EBC2EDD7_C2AC_4519_9357_CE387B50513F

#include <ctp/ThostFtdcTraderApi.h>
#include <stdlib.h>
#include <string.h>

namespace ctp {
struct ref_t {
    explicit ref_t( const TThostFtdcOrderRefType& ctp_ref_ );
    explicit ref_t( const char* sz_ref_ );
    explicit ref_t( unsigned ref_ );
    explicit ref_t( const ref_t& r_ );
    ref_t();

    void        copy( TThostFtdcOrderRefType& ref_ );
    ref_t&      operator=( const ref_t& r_ );
    ref_t&      operator+=( int diff_ );
    ref_t&      operator++();
    ref_t       operator+( int diff_ );
    const char* str_val() const;
    unsigned    int_val() const;

private:
    void from( unsigned v_ );

private:
    enum {
        DATA_LENGTH = sizeof( TThostFtdcOrderRefType ) + 1
    };

    char _data[ DATA_LENGTH ];
};

ref_t::ref_t( const TThostFtdcOrderRefType& ctp_ref_ ) {
    memcpy( _data, ctp_ref_, sizeof( TThostFtdcOrderRefType ) );
    _data[ DATA_LENGTH - 1 ] = 0;
}

ref_t::ref_t( const char* sz_ref_ )
    : ref_t( atoi( sz_ref_ ) ) {
}

void ref_t::from( unsigned v_ ) {
    // todo
    memset( _data, '0', sizeof( _data ) );

    int i = DATA_LENGTH - 1;

    _data[ i-- ] = 0;
    while ( v_ && i >= 0 ) {
        _data[ i-- ] = v_ % 10 + '0';
        v_ /= 10;
    }
}

ref_t::ref_t( unsigned ref_ ) {
    from( ref_ );
}

ref_t::ref_t( const ref_t& r_ )
    : ref_t( r_.int_val() ) {}

ref_t::ref_t() {
    memset( _data, '0', DATA_LENGTH );
    _data[ DATA_LENGTH - 1 ] = 0;
}

ref_t& ref_t::operator++() {
    from( int_val() + 1 );
    return *this;
}

ref_t& ref_t::operator=( const ref_t& r_ ) {
    memcpy( _data, r_._data, sizeof( _data ) );
    return *this;
}

ref_t& ref_t::operator+=( int diff_ ) {
    from( int_val() + diff_ );
    return *this;
}

ref_t ref_t::operator+( int diff_ ) {
    return ref_t( int_val() + diff_ );
}

const char* ref_t::str_val() const {
    return _data;
}

unsigned ref_t::int_val() const {
    return std::atoi( _data );
}

};  // namespace ctp

#endif /* EBC2EDD7_C2AC_4519_9357_CE387B50513F */
