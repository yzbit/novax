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

#ifndef EB26CA36_71A3_4274_998C_7AA18A5F113A
#define EB26CA36_71A3_4274_998C_7AA18A5F113A
/** note!! 使用前仔细阅读brief
 * @file logz.h
 * @author y
 * @brief 开发阶段在sr日志没工作之前临时做的一个轻量级日志。
 * !! [ 线程安全 ]
 * !! [ 没有cache ] 如果模块对性能有很高要求要评估一下是否可以用！！
 * @version 0.1
 * @date 2023-04-20
 *
 */

#include <chrono>
#include <fcntl.h>
#include <libgen.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

#include "ns.h"

NVX_NS_BEGIN

struct Logz {
    enum class severty_t {
        info,
        warn,
        error,
        fatal
    };

    static Logz& instance();

    int shut();

    // log_file_ 包含路径
    int  init( const char* log_file_, int rotate_, bool keep_ );
    int  lite( const char* msg_ );
    int  details( const char* tag_, const char* file_, const char* func_, int line_, const char* fmt_, ... );
    int  fmt( const char* fmt_, ... );
    int  archive( const char* file_ );
    void flush();
    void set_stdout( bool enable_ = true ) { _use_stdout = enable_; }

private:
    std::string time_str( bool simple_ = false );
    std::string real_log( const std::string& name_ );
    std::string time_str_file();

private:
    bool        _use_stdout       = false;
    bool        _keep             = false;
    std::string _log_file         = "";
    int         _log_fd           = -1;
    int32_t     _log_size         = 0;
    int         _rotate_threshold = kMaxRotate;

    static constexpr int32_t kMaxRotate = 5 * 1024 * 1024;
};

inline Logz& Logz::instance() {
    static Logz l;
    return l;
}

NVX_NS_END

#define LOGZ NVX_NS::Logz::instance()
#define SR_LOGZ_CLOSE LOGZ.shut

#ifdef MS_VC
#define _filename_( x ) strrchr( x, '\\' ) ? strrchr( x, '\\' ) + 1 : x
#else
#define _filename_( _x_ ) strrchr( _x_, '/' ) ? strrchr( _x_, '/' ) + 1 : _x_
#endif

#define trace_intl( _fmt_, ... ) LOGZ.details( nullptr, _filename_( __FILE__ ), __FUNCTION__, __LINE__, _fmt_, ##__VA_ARGS__ )

#define LOG_ERROR trace_intl
#define LOG_WARN trace_intl
#define LOG_INFO trace_intl

#define LOG_TAGGED( _tag_, _fmt_, ... ) LOGZ.details( _tag_, _filename_( __FILE__ ), __FUNCTION__, __LINE__, _fmt_, ##__VA_ARGS__ )

#define LOG_FLUSH LOGZ.flush
#define LOG_INIT( _name_, _rotate_ ) \
    LOGZ.init( _name_, _rotate_, false )

#define LOG_INIT_KEEP( _name_, _rotate_ ) \
    LOGZ.init( _name_, _rotate_, true )

#define LOG_TRACE LOGZ.fmt

#define LOG_DISABLE_STDOUT() LOGZ.set_stdout( false )
#define LOG_ENABLE_STDOUT() LOGZ.set_stdout( true )

// #define LOGZ_AR(_file_) sr_utility::lgz_archive(_file_)

#define LOG_FATAL( ... )                  \
    do {                                  \
        fprintf( stderr, ##__VA_ARGS__ ); \
        exit( -1 );                       \
    } while ( 0 )

NVX_NS_BEGIN

inline std::string Logz::time_str( bool simple_ ) {
    auto now = std::chrono::system_clock::now();
    // 通过不同精度获取相差的毫秒数
    uint64_t dis_millseconds =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch() )
            .count()
        - std::chrono::duration_cast<std::chrono::seconds>( now.time_since_epoch() )
                  .count()
              * 1000;
    time_t tt             = std::chrono::system_clock::to_time_t( now );
    auto   time_tm        = localtime( &tt );
    char   str_time[ 64 ] = { 0 };

    if ( !simple_ )
        sprintf( str_time, "[ %d-%02d-%02d %02d:%02d:%02d.%03d ]", time_tm->tm_year + 1900, time_tm->tm_mon + 1, time_tm->tm_mday, time_tm->tm_hour, time_tm->tm_min, time_tm->tm_sec, ( int )dis_millseconds );

    else
        sprintf( str_time, "[ %02d:%02d:%02d.%03d ]", time_tm->tm_hour, time_tm->tm_min, time_tm->tm_sec, ( int )dis_millseconds );

    return str_time;
}

inline std::string Logz::time_str_file() {
    auto now = std::chrono::system_clock::now();

    time_t tt             = std::chrono::system_clock::to_time_t( now );
    auto   time_tm        = localtime( &tt );
    char   str_time[ 64 ] = { 0 };

    sprintf( str_time, "%02d%02d%02d-%02d%02d%02d", time_tm->tm_year + 1900 - 2000, time_tm->tm_mon + 1, time_tm->tm_mday, time_tm->tm_hour, time_tm->tm_min, time_tm->tm_sec );

    printf( "time_str_file:%s\n", str_time );
    return str_time;
}

inline std::string Logz::real_log( const std::string& name_ ) {
    if ( _keep ) return name_;

    std::string real = name_ + std::string( "-" ) + time_str_file() + ".log";
    return real;
}

inline void Logz::flush() {
    if ( _log_fd < 0 )
        return;
    fsync( _log_fd );
}

inline int Logz::shut() {
    close( _log_fd );
    return 0;
}

inline int Logz::lite( const char* msg_ ) {
    if ( _use_stdout ) {
        fprintf( stderr, "%s", msg_ );
    }

    if ( _log_fd < 0 ) {
        return -1;
    }

    if ( _log_size > _rotate_threshold ) {
        printf( "----------logger rotating---------\n" );
        if ( _keep ) {
            std::string real = this->real_log( _log_file );
            std::string n    = real + ".00";
            rename( real.c_str(), n.c_str() );

            auto fd = open( real.c_str(), O_CREAT | O_RDWR, 0666 );
            dup2( fd, _log_fd );
            close( fd );

            remove( n.c_str() );
        }
        else {
            std::string real = this->real_log( _log_file );
            auto        fd   = open( real.c_str(), O_CREAT | O_RDWR, 0666 );
            dup2( fd, _log_fd );
            close( fd );
        }

        _log_size = 0;
    }

    size_t sz = strlen( msg_ );

    // if ( sz >= 2 && msg_[ sz - 1 ] == '\n' && msg_[ sz - 2 ] == '\n' )
    //    sz = write( _log_fd, msg_, strlen( msg_ ) - 1 );
    // else

    sz = write( _log_fd, msg_, strlen( msg_ ) );

    _log_size += sz;

#ifdef LGZ_DEBUG
    fsync( _log_fd );
#endif

    return sz;
}

inline int Logz::details( const char* tag_, const char* file_, const char* func_, int line_, const char* fmt_, ... ) {
    char buff[ 1024 ];
    memset( buff, 0x00, sizeof( buff ) );

    int n = 0;
    if ( tag_ && strlen( tag_ ) > 0 )
        n = sprintf( buff, "#%s, %s [ %s:%s@%d ]", tag_, time_str().c_str(), file_, func_, line_ );
    else
        n = sprintf( buff, "%s [ %s:%s@%d ]", time_str().c_str(), file_, func_, line_ );

    va_list ap;
    va_start( ap, fmt_ );
    vsnprintf( buff + n, 1024 - n - 1, fmt_, ap );  // should -n -1,for \n todo
    va_end( ap );

    if ( strlen( buff ) < 1 || buff[ strlen( buff ) - 1 ] != '\n' ) {
        buff[ strlen( buff ) ] = '\n';
    }

    return this->lite( buff );
}

inline int Logz::fmt( const char* fmt_, ... ) {
    char buff[ 1024 ];

    va_list ap;
    va_start( ap, fmt_ );
    vsnprintf( buff, 1024, fmt_, ap );
    va_end( ap );

    return this->lite( buff );
}

inline int Logz::archive( const char* file_ ) {
    close( _log_fd );
    return 0;
}

inline int Logz::init( const char* log_file_, int rotate_, bool keep_ ) {
    if ( _log_fd > 0 )
        return 0;

    _keep     = keep_;
    _log_file = log_file_;
    if ( rotate_ > 0 )
        _rotate_threshold = rotate_;

    printf( "--lgz--init-%s, rotate=%d\n", _log_file.c_str(), _rotate_threshold );

    auto path = ::strdup( _log_file.c_str() );
    auto dir  = dirname( path );

    if ( strcmp( dir, "." ) && strcmp( dir, ".." ) && ::access( dir, F_OK ) < 0 ) {
        char cmd[ 256 ];
        sprintf( cmd, "mkdir -p %s", dir );
        ::system( cmd );
    }

    // if ( remove_old_ ) ::remove( _log_file.c_str() );
    free( path );

    std::string real = this->real_log( _log_file );
    _log_fd          = open( real.c_str(), O_CREAT | O_RDWR, 0666 );

    lseek( _log_fd, 0, SEEK_END );

    std::thread( [ & ]() {
        for ( ;; ) {
            this->flush();
            sleep( 5 );
        }
    } )
        .detach();

    return 0;
}

NVX_NS_END

#endif /* EB26CA36_71A3_4274_998C_7AA18A5F113A */
