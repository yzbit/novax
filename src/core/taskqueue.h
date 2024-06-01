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

#ifndef B2AD377C_2C34_45B8_829B_C12AABBC4481
#define B2AD377C_2C34_45B8_829B_C12AABBC4481
#include <assert.h>
#include <condition_variable>
#include <functional>
#include <list>
#include <mutex>
#include <thread>
#include <vector>

#include "log.hpp"
#include "ns.h"

// #define DBG_TRACE fprintf
#define DBG_TRACE( ... )

NVX_NS_BEGIN
struct task_queue {
    task_queue()          = default;
    virtual ~task_queue() = default;

    static task_queue* create( int _poolsize_ );

    template <typename _Callable, typename... _Args>
    void run_r( _Callable&& __f, _Args&&... __args ) {
        run( [ & ]() {
            std::forward<_Callable>( __f )( std::forward<_Args>( __args )... );
        } );
    }

    virtual void drain()                         = 0;
    virtual bool busy()                          = 0;
    virtual void run( std::function<void()> fn ) = 0;
    virtual void shutdown()                      = 0;
    virtual void on_idle() {}
};

struct ThreadPool : public task_queue {
    explicit ThreadPool( size_t n )
        : _shutdown( false ) {
        while ( n ) {
            _threads.emplace_back( worker( *this ) );
            n--;
        }
    }

    ThreadPool( const ThreadPool& ) = delete;
    ~ThreadPool() override          = default;

    void run( std::function<void()> fn ) override {
        {
            std::unique_lock<std::mutex> lock( _mutex );
            _jobs.push_back( std::move( fn ) );

            // LOG_TAGGED( "taskq", "[0x%x] add jobs ,current=%d", ( intptr_t )this, ( int )_jobs.size() );
        }

        _cond.notify_one();
    }

    virtual void drain() {
        std::unique_lock<std::mutex> lock( _mutex );
        _jobs.clear();

        DBG_TRACE( stderr, "current idle workers=%d", _idles );
    }

    void shutdown() override {
        {
            LOG_TAGGED( "taskq", "[0x%x] shutdown", ( intptr_t )this );
            std::unique_lock<std::mutex> lock( _mutex );
            _shutdown = true;
        }

        _cond.notify_all();

        for ( auto& t : _threads ) {
            t.join();
        }
    }

    bool busy() override {
        DBG_TRACE( stderr, "idles=%d\n", _idles );
        return _idles != ( int )_threads.size() && !_shutdown;
    }

private:
    struct worker {
        explicit worker( ThreadPool& pool )
            : _pool( pool ) {}

        void operator()() {
            for ( ;; ) {
                std::function<void()> fn;

                {
                    // DBG_TRACE( stderr, "### spawn task jobs\n" );
                    std::unique_lock<std::mutex> lock( _pool._mutex );
                    ++_pool._idles;

                    _pool._cond.wait( lock, [ & ] { return !_pool._jobs.empty() || _pool._shutdown; } );

                    // DBG_TRACE( stderr, "###--wait release\n" );
                    if ( _pool._shutdown && _pool._jobs.empty() ) {
                        break;
                    }

                    --_pool._idles;

                    // LOG_TAGGED( "taskq", "[0x%x] pick one task to run, backlogs=%d", ( intptr_t )this, ( int )_pool._jobs.size() );
                    fn = std::move( _pool._jobs.front() );
                    _pool._jobs.pop_front();
                }

                NVX_ASSERT( true == static_cast<bool>( fn ) );
                fn();
            }  // for
        }

        ThreadPool& _pool;
    };
    friend struct worker;

    std::vector<std::thread>         _threads;
    std::list<std::function<void()>> _jobs;

    bool                    _shutdown;
    int                     _idles = 0;
    std::condition_variable _cond;
    std::mutex              _mutex;
};

inline task_queue* task_queue::create( int _poolsize_ ) {
    return new ThreadPool( _poolsize_ );
}

NVX_NS_END

#endif /* B2AD377C_2C34_45B8_829B_C12AABBC4481 */
