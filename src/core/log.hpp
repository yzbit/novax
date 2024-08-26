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
#include <future>
#include <libgen.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <unistd.h>

#include "ns.h"

NVX_NS_BEGIN
struct logz {
    static constexpr size_t ROTATE_DEFAULT = 5 * 1024 * 1024;

    struct conf {
        size_t      rot_size;    //! 单个日志文件大小
        bool        keep;        //! 是否保留旧日志
        bool        use_stdout;  //! 是否输出到标准输出
        std::string dir;         //! 日志目录
        std::string prefix;      //! 日志文件名前缀
        int         time_fmt;    //! 时间格式-0: ms count 1: 2023-04-20 12:34:56.789
    };

    static logz& instance() {
        static logz l;
        return l;
    }

    int shut() {
        close( _log_fd );
        return 0;
    }

    const char* real() {
        static std::string name = "";
        if ( _c.keep ) {
            name = _c.dir + _c.prefix + ".log";
        }
        else {
            name = _c.dir + _c.prefix + "." + time_str_file() + ".log";
        }

        return name.c_str();
    }

    int init( const conf& c_ ) {
        if ( _log_fd > 0 )
            return 0;

        _c = c_;

        if ( _c.rot_size <= 256 ) {
            _c.rot_size = ROTATE_DEFAULT;
        }

        printf( "lgz--init: dir=%s,prefix=%s,rotate=%lu ,keep=%d, use_stdout=%d, time_fmt=%d\n",
                _c.dir.c_str(),
                _c.prefix.c_str(),
                _c.rot_size,
                _c.keep,
                _c.use_stdout,
                _c.time_fmt );

        // todo: only linux
        if ( _c.dir.back() != '/' ) _c.dir += "/";

        if ( ::access( _c.dir.c_str(), F_OK ) < 0 ) {
            printf( "mkdir %s\n", _c.dir.c_str() );

            char cmd[ 256 ];
            sprintf( cmd, "mkdir -p %s", _c.dir.c_str() );
            ::system( cmd );
        }

        _log_fd = open( real(), O_CREAT | O_RDWR, 0666 );
        lseek( _log_fd, 0, SEEK_END );
        [[maybe_unused]] static auto fut = std::async( std::launch::async, [ & ]() { for ( ;; ) { this->flush(); sleep( 5 ); } } );
        // fut.detach();
        puts( "logz init done" );

        return 0;
    }

    void lite( const char* msg_ ) {
        if ( _c.use_stdout ) {
            fprintf( stderr, "%s", msg_ );
        }

        if ( _log_fd < 0 ) {
            fprintf( stderr, "log init failed\n" );
            return;
        }

        if ( _log_size > _c.rot_size ) {
            printf( "logger rotating: %lu\n", _log_size );
            _log_size = 0;

            const char* f = real();
            printf( "logger name: %s\n", f );

            if ( _c.keep ) {
                std::string bak = std::string( real() ) + ".bak";
                const char* rf  = bak.c_str();

                rename( f, rf );
                auto fd = open( f, O_CREAT | O_RDWR, 0666 );
                dup2( fd, _log_fd );
                close( fd );

                remove( rf );
            }
            else {
                auto fd = open( f, O_CREAT | O_RDWR, 0666 );
                dup2( fd, _log_fd );
                close( fd );
            }
        }

        _log_size += write( _log_fd, msg_, strlen( msg_ ) );

#ifdef LGZ_DEBUG
        fsync( _log_fd );
#endif
    }

    void detail( const char* fmt_, ... ) {
        char buff[ 1024 ] = { 0 };

        auto n = sprintf( buff, "[%s] ", time_str() );

        va_list ap;
        va_start( ap, fmt_ );
        vsnprintf( buff + n, 1024 - n - 1, fmt_, ap );  // should -n -1,for \n todo
        va_end( ap );

        if ( strlen( buff ) < 1 || buff[ strlen( buff ) - 1 ] != '\n' ) {
            buff[ strlen( buff ) ] = '\n';
        }

        this->lite( buff );
    }

    void flush() {
        if ( _log_fd > 0 ) {
            fsync( _log_fd );
        }
    }

    void set_stdout( bool enable_ = true ) { _c.use_stdout = enable_; }

private:
    char* time_str( bool simple_ = false ) {
        static char str_time[ 64 ] = { 0 };

        auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() ).count();

        if ( _c.time_fmt == 0 ) {
            sprintf( str_time, "%ld", now_ms );
            return str_time;
        }

        auto now = std::chrono::system_clock::now();

        time_t tt      = std::chrono::system_clock::to_time_t( now );
        auto   time_tm = localtime( &tt );

        if ( !simple_ )
            sprintf( str_time, "[ %d-%02d-%02d %02d:%02d:%02d.%03d ]", time_tm->tm_year + 1900, time_tm->tm_mon + 1, time_tm->tm_mday, time_tm->tm_hour, time_tm->tm_min, time_tm->tm_sec, ( int )now_ms % 1000 );

        else
            sprintf( str_time, "[ %02d:%02d:%02d.%03d ]", time_tm->tm_hour, time_tm->tm_min, time_tm->tm_sec, ( int )now_ms % 1000 );

        return str_time;
    }

    std::string time_str_file() {
        auto now = std::chrono::system_clock::now();

        time_t tt             = std::chrono::system_clock::to_time_t( now );
        auto   time_tm        = localtime( &tt );
        char   str_time[ 64 ] = { 0 };

        sprintf( str_time, "%02d%02d%02d-%02d%02d%02d", time_tm->tm_year + 1900 - 2000, time_tm->tm_mon + 1, time_tm->tm_mday, time_tm->tm_hour, time_tm->tm_min, time_tm->tm_sec );

        printf( "time_str_file:%s\n", str_time );
        return str_time;
    }

private:
    int         _log_fd = -1;
    conf        _c;
    std::string _log_file = "";
    size_t      _log_size = 0;
};

NVX_NS_END

#ifdef MS_VC
#define FILE_NAME( x ) strrchr( x, '\\' ) ? strrchr( x, '\\' ) + 1 : x
#else
#define FILE_NAME( _x_ ) strrchr( _x_, '/' ) ? strrchr( _x_, '/' ) + 1 : _x_
#endif

#define LOGZ NVX_NS::logz::instance()
#define LOGZ_INIT( c ) LOGZ.init( c )
#define LOGZ_CLOSE() LOGZ.shut()

#define LOGZ_RAW( msg ) LOGZ.lite( msg )
#define LOGZ_FMT( fmt, ... ) LOGZ.detail( "%s@%s:%d" fmt, FILE_NAME( __FILE__ ), __FUNCTION__, __LINE__, ##__VA_ARGS__ )
#define LOGZ_FLUSH() LOGZ.flush()

#define LOGZ_DISABLE_STDOUT() LOGZ.set_stdout( false )
#define LOGZ_ENABLE_STDOUT() LOGZ.set_stdout( true )
#endif