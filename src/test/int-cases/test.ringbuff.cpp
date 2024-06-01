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

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <unistd.h>

#include "../core/definitions.h"
#include "../core/models.h"
#include "../core/ring_buffer.h"

int main() {
    std::cout << "file name=" << __FILE__ << std::endl;

    cub::ring_buff<cub::tick, 5> ring;

    cub::tick r, w;

    std::condition_variable cv;
    std::mutex              mtx;

    THREAD_DETACHED( [ & ]() {
        cub::price p = 0;
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