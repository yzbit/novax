#ifndef E45467B2_B006_4410_B395_0CD67CCB3C3F
#define E45467B2_B006_4410_B395_0CD67CCB3C3F

#include <condition_variable>
#include <future>
#include <mutex>

#include "../ns.h"
#include "comm.h"

NVX_NS_BEGIN

namespace ctp {
struct gateway {
    virtual void on_init()    = 0;
    virtual void on_release() = 0;

protected:
    bool is_running() {
        return _running;
    }

    void respawn() {
        _respawn = true;
        std::unique_lock<std::mutex> lck{ _mutex };
        _cv.notify_one();
    }

    void teardown() {
        if ( !_running ) return;

        _running = false;
        std::unique_lock<std::mutex> lck{ _mutex };
        _cv.notify_one();
        _fut.wait();
    }

    void daemon() {
        _fut = std::move( std::async( std::launch::async, &gateway::run, this ) );
    }

private:
    void run() {
        while ( _running ) {
            if ( !in_trade_time() ) {
                std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
                continue;
            }

            on_init();

            std::unique_lock<std::mutex> lck{ _mutex };
            _cv.wait( lck, [ & ]() { return !_running || _respawn; } );

            on_release();
        }
    }

private:
    bool              _respawn = false;
    bool              _running = true;
    std::mutex        _mutex;
    std::future<void> _fut;

    std::condition_variable _cv;
};
};  // namespace ctp

NVX_NS_END

#endif /* E45467B2_B006_4410_B395_0CD67CCB3C3F */
