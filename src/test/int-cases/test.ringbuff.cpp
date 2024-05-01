#include <condition_variable>
#include <iostream>
#include <mutex>
#include <unistd.h>

#include "../core/definitions.h"
#include "../core/models.h"
#include "../core/ring_buffer.h"

int main() {
    std::cout << "file name=" << __FILE__ << std::endl;

    cub::RingBuff<cub::quotation_t, 5> ring;

    cub::quotation_t r, w;

    std::condition_variable cv;
    std::mutex              mtx;

    THREAD_DETACHED( [ & ]() {
        cub::price_t p = 0;
        while ( 1 ) {
            w.ask = ++p;
            ring.put( w );

            if ( ring.size() > 0 ) {
                std::unique_lock<std::mutex> lock{ mtx };
                cv.notify_one();
            }

            sleep( 1 );
        }
    } );

    THREAD_JOINED( [ & ]() {
        auto wait = [ & ]() {
            std::unique_lock<std::mutex> lock{ mtx };
            cv.wait( lock, [ & ]() { return ring.size() > 0; } );
        };

        while ( 1 ) {
            wait();

            std::cout << "r size=" << ring.size() << std::endl;
            if ( ring.pop( r ) < 0 ) {
                // std::this_thread::yield();
                // continue;
                assert( 0 );
            }

            if ( r.ask < 7 ) {  // 先慢后快，中间会丢失几个值,10 12会丢失
                sleep( 2 );
            }
            std::cout << "r:value = " << r.ask << std::endl;
        }
    } );

    return 0;
}