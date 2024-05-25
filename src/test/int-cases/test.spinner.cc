#include <atomic>
#include <mutex>
#include <optional>
#include <stdio.h>
#include <thread>

struct Spinner {
    void lock() {
        bool exp = false;

        while ( !_flag.compare_exchange_weak( exp, true, std::memory_order::memory_order_release, std::memory_order::memory_order_relaxed ) )
            exp = false;
    }

    void unlock() {
        _flag.store( false, std ::memory_order_release );
    }

private:
    std::atomic_bool _flag = false;
};

int main() {
    std::optional<int> opt;
    if ( !opt ) {
        printf( "no value\n" );
    }
    opt = 2;
    if ( !opt ) {
        printf( "no value2\n" );
    }
    else {
        printf( "have value2\n" );
    }

    int     i = 0;
    Spinner sp;

    std::thread t1( [ & ]() {
        for ( int j = 0; j < 1000000; ++j ) {
            std::unique_lock<Spinner> _lck{ sp };
            ++i;
        }
    } );

    std::thread t2( [ & ]() {
        for ( int j = 0; j < 1000000; ++j ) {
            std::unique_lock<Spinner> _lck{ sp };
            ++i;
        }
    } );

    std::thread t3( [ & ]() {
        for ( int j = 0; j < 1000000; ++j ) {
            std::unique_lock<Spinner> _lck{ sp };
            ++i;
        }
    } );

    t1.join();
    t2.join();
    t3.join();

    printf( "%d\n", i );
    return 0;
}