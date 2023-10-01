#ifndef B2AD377C_2C34_45B8_829B_C12AABBC4481
#define B2AD377C_2C34_45B8_829B_C12AABBC4481
#include <assert.h>
#include <condition_variable>
#include <cub_ns.h>
#include <functional>
#include <list>
#include <mutex>
#include <thread>
#include <vector>

CUB_NS_BEGIN
struct TaskQueue {
    TaskQueue()          = default;
    virtual ~TaskQueue() = default;

    static TaskQueue* create( int _poolsize_ );

    template <typename _Callable, typename... _Args>
    void run_r( _Callable&& __f, _Args&&... __args ) {
        run( [ & ]() {
            std::forward<_Callable>( __f )( std::forward<_Args>( __args )... );
        } );
    }

    virtual bool busy()                          = 0;
    virtual void run( std::function<void()> fn ) = 0;
    virtual void shutdown()                      = 0;
    virtual void on_idle() {}
};

struct ThreadPool : public TaskQueue {
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
        }

        _cond.notify_one();
    }

    void shutdown() override {
        {
            std::unique_lock<std::mutex> lock( _mutex );
            _shutdown = true;
        }

        _cond.notify_all();

        for ( auto& t : _threads ) {
            t.join();
        }
    }

    bool busy() override {
        fprintf( stderr, "idles=%d\n", _idles );
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
                    std::unique_lock<std::mutex> lock( _pool._mutex );
                    ++_pool._idles;

                    _pool._cond.wait(
                        lock, [ & ] { return !_pool._jobs.empty() || _pool._shutdown; } );

                    if ( _pool._shutdown && _pool._jobs.empty() ) {
                        break;
                    }

                    --_pool._idles;

                    fn = std::move( _pool._jobs.front() );
                    _pool._jobs.pop_front();
                }

                assert( true == static_cast<bool>( fn ) );
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

inline TaskQueue* TaskQueue::create( int _poolsize_ ) {
    return new ThreadPool( _poolsize_ );
}

CUB_NS_END

#endif /* B2AD377C_2C34_45B8_829B_C12AABBC4481 */
