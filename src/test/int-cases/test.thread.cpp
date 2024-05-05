#include <stdio.h>
#include <thread>
#include <unistd.h>
int main() {

    std::thread( []() {
        for ( ;; ) {
            ::sleep( 1 );
            printf( "on thread\n" );
        }
    } ).join();
    return 0;
}