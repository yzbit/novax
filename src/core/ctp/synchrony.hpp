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

#ifndef C54C1177_02ED_410B_A390_8C14D20A42F5
#define C54C1177_02ED_410B_A390_8C14D20A42F5

#include <algorithm>
#include <condition_variable>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <stdint.h>
#include <string.h>

#include "../ns.h"

NVX_NS_BEGIN
namespace ctp {
struct Synchrony {
    struct block_t final {
        char*  data;
        size_t size;

        block_t();
        ~block_t();
        block_t( const void* data_, size_t sz_ );
        block_t( const block_t& b_ );
        block_t( block_t&& b_ );
        block_t& operator=( const block_t& b_ );
        block_t& operator=( block_t&& b_ );
    };

    using seglist_t = std::list<block_t>;

    struct seg_t final {
        seglist_t data;

        seg_t() = default;
        seg_t( seg_t&& s_ );
        seg_t( const seg_t& s_ );
        size_t size() const;
        seg_t& operator=( seg_t&& s_ );
        seg_t& operator=( const seg_t& s_ );
    };

    struct entry_t final {
        volatile bool               finish = false;
        seg_t                       segments;
        std::unique_ptr<std::mutex> mutex;

        std::unique_ptr<std::condition_variable> cv;

        entry_t();
        entry_t( entry_t&& e_ );
    };

    static Synchrony& get();

    void erase( int id_ );
    void update( int id_, const void* data_, size_t size_, bool finish_ );
    template <typename OBJPTR, typename FUNC, typename... ARGS>
    seg_t wait( int id_, uint32_t timeout_ms_, OBJPTR o_, FUNC f_, ARGS&&... a_ ) {
        auto&    sync = Synchrony::get();
        entry_t* e    = sync.put( id_ );
        if ( !e ) return seg_t();

        int rc = ( o_->*f_ )( std::forward<ARGS>( a_ )... );
        if ( rc ) return seg_t();

        std::unique_lock<std::mutex> lock( *e->mutex );

        seg_t res = std::move( e->segments );

        if ( e->finish ) {
            sync.erase( id_ );
            return res;
        }

        if ( !e->cv->wait_for( lock, std::chrono::milliseconds( timeout_ms_ ), [ = ]() { return e->finish; } )
             && !e->mutex->try_lock() ) {
            e->mutex->lock();
            res = seg_t();
        }

        sync.erase( id_ );

        return res;
    }

private:
    entry_t* put( int id_ );
    entry_t* fetch( int id_ );

private:
    std::mutex             _mutex;
    std::map<int, entry_t> _data;
};

//----inline implemets.
inline Synchrony::block_t::block_t( const block_t& b_ )
    : size{ b_.size } {
    data = new char[ size ];
    memcpy( data, b_.data, size );
}

inline Synchrony::block_t::block_t( block_t&& b_ )
    : size{ b_.size } {
    std::swap( data, b_.data );
    b_.data = nullptr;
}

inline Synchrony::block_t::block_t()
    : data{ nullptr }
    , size{ 0u } {}

inline Synchrony::block_t::block_t( const void* data_, size_t sz_ )
    : data{ nullptr }
    , size{ sz_ } {
    data = new char[ sz_ ];
    memcpy( data, data_, sz_ );
}

inline Synchrony::block_t& Synchrony::block_t::operator=( const block_t& b_ ) {
    size = b_.size;
    data = new char[ size ];
    memcpy( data, b_.data, size );

    return *this;
}

inline Synchrony::block_t& Synchrony::block_t::operator=( block_t&& b_ ) {
    size = b_.size;
    std::swap( data, b_.data );
    b_.data = nullptr;
    return *this;
}

inline Synchrony::block_t::~block_t() {
    delete[] data;
}

//------seg-t
inline Synchrony& Synchrony::get() {
    static Synchrony _instance;

    return _instance;
}

inline Synchrony::entry_t::entry_t() {
    mutex = std::make_unique<std::mutex>();
    cv    = std::make_unique<std::condition_variable>();
}

inline Synchrony::entry_t::entry_t( entry_t&& e_ ) {
    mutex.swap( e_.mutex );
    cv.swap( e_.cv );
}

inline Synchrony::entry_t* Synchrony::put( int id_ ) {
    std::unique_lock<std::mutex> lock{ _mutex };
    _data.try_emplace( id_, Synchrony::entry_t() );

    return &_data[ id_ ];
}

inline Synchrony::entry_t* Synchrony::fetch( int id_ ) {
    return _data.find( id_ ) == _data.end()
               ? nullptr
               : &_data[ id_ ];
}

inline void Synchrony::update( int id_, const void* data_, size_t size_, bool finish_ ) {
    if ( !data_ || size_ == 0 ) return;

    entry_t* e = fetch( id_ );
    if ( !e ) return;

    std::unique_lock<std::mutex> lock{ *e->mutex.get() };

    e->segments.data.push_back( { data_, size_ } );
    e->finish = finish_;

    if ( finish_ ) e->cv->notify_one();
}

inline void Synchrony::erase( int id_ ) {
    std::unique_lock<std::mutex> lock{ _mutex };
    _data.erase( id_ );
}

inline Synchrony::seg_t::seg_t( seg_t&& s_ ) {
    for ( auto& v : s_.data ) {
        data.push_back( std::move( v ) );
    }

    s_.data.clear();
}

inline size_t Synchrony::seg_t::size() const {
    return data.size();
}

inline Synchrony::seg_t& Synchrony::seg_t::operator=( seg_t&& s_ ) {
    for ( auto& v : s_.data ) {
        data.push_back( std::move( v ) );
    }

    s_.data.clear();
    return *this;
}

inline Synchrony::seg_t& Synchrony::seg_t::operator=( const seg_t& s_ ) {
    for ( const auto& v : s_.data ) {
        data.push_back( v );
    }

    return *this;
}

inline Synchrony::seg_t::seg_t( const seg_t& s_ ) {
    for ( const auto& v : s_.data ) {
        data.push_back( v );
    }
}

#define CTP_SYNC Synchrony::get()
}  // namespace ctp

#endif /* C54C1177_02ED_410B_A390_8C14D20A42F5 */
NVX_NS_END