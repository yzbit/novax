#include "comm.h"

CUB_NS_BEGIN
namespace ctp {

Synchrony& Synchrony::get() {
    static Synchrony _instance;

    return _instance;
}

Synchrony::entry_t* Synchrony::put( int id_ ) {
    std::unique_lock<std::mutex> lock{ _mutex };
    _data.try_emplace( id_, Synchrony::entry_t() );

    return &_data[ id_ ];
}

Synchrony::entry_t* Synchrony::fetch( int id_ ) {
    return _data.find( id_ ) == _data.end()
               ? nullptr
               : &_data[ id_ ];
}

void Synchrony::update( int id_, const void* data_, size_t size_, bool finish_ ) {
    if ( !data_ || size_ == 0 ) return;

    entry_t* e = fetch( id_ );
    if ( !e ) return;

    std::unique_lock<std::mutex> lock{ e->mutex };

    char* data = new char[ size_ ]( 0 );
    memcpy( data, data_, size_ );
    e->segments.push_back( data );
    e->finish = finish_;

    if ( finish_ ) {
        e->cv.notify_one();
    }
}
}  // namespace ctp

CUB_NS_END