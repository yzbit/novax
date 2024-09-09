/*
BSD 3-Clause License

Copyright (c) 2024, YaoZinan

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

wechat:371536435 email:yzbit@outlook.com
*/

#ifndef EB26CA36_71A3_4274_998C_7AA18A5F113A
#define EB26CA36_71A3_4274_998C_7AA18A5F113A

#include <chrono>
#include <fcntl.h>
#include <future>
#include <libgen.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/file.h>
#include <sys/types.h>
#include <unistd.h>

namespace yy {
struct logz {
    static constexpr size_t ROTATE_DEFAULT = 5 * 1024 * 1024;

    struct conf {
        size_t      rot_size;    //! 单个日志文件大小
        bool        in_place;    //! 原地轮换
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
                _c.in_place,
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

        _log_size = lseek( _log_fd, 0, SEEK_CUR );

        //[[maybe_unused]] static auto fut =
        //    std::async( std::launch::async, [ & ]() { for ( ;; ) { this->flush(); sleep( 5 ); } } );

        puts( "logz init done" );

        return 0;
    }

    void lite( const char* msg_ ) {
        if ( _c.use_stdout ) {
            printf( "%s", msg_ );
        }

        if ( _log_fd < 0 ) {
            fprintf( stderr, "log init failed\n" );
            exit( -1 );
        }

        if ( _log_size > _c.rot_size ) {
            printf( "logger rotating: %lu\n", _log_size );

            _log_size = 0;

            const char* f = real();
            printf( "logger name: %s\n", f );

            if ( _c.in_place ) {
                std::string bak = std::string( real() ) + ".1";
                const char* rf  = bak.c_str();

                rename( f, rf );
                auto fd = open( f, O_CREAT | O_RDWR, 0666 );
                dup2( fd, _log_fd );
                close( fd );
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
    const char* real() {
        static std::string name = "";
        if ( _c.in_place ) {
            name = _c.dir + _c.prefix + ".log";
        }
        else {
            name = _c.dir + _c.prefix + "." + time_str_file() + ".log";
        }

        return name.c_str();
    }

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

}  // namespace yy

#define FILE_NAME( full_path ) strrchr( full_path, '/' ) ? strrchr( full_path, '/' ) + 1 : full_path

#define LOGZ yy::logz::instance()
#define LOGZ_INIT( c ) LOGZ.init( c )
#define LOGZ_CLOSE() LOGZ.shut()

#define LOGZ_MSG( msg ) LOGZ.lite( msg )
#define LOGZ_FMT( fmt, ... ) LOGZ.detail( "%s@%s:%d> " fmt, FILE_NAME( __FILE__ ), __FUNCTION__, __LINE__, ##__VA_ARGS__ )
#define LOGZ_FLUSH() LOGZ.flush()

#define LOGZ_DISABLE_STDOUT() LOGZ.set_stdout( false )
#define LOGZ_ENABLE_STDOUT() LOGZ.set_stdout( true )

#define LOG_INFO LOGZ_FMT

#endif /* C4D939F4_A0F4_45D4_ACE6_CC6779AF8D29 */
