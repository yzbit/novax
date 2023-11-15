#ifndef B43732C7_EA9D_4138_8023_E0627CD66A48
#define B43732C7_EA9D_4138_8023_E0627CD66A48
#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <fstream>
#include <functional>
#include <list>
#include <math.h>
#include <memory>
#include <mutex>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <sstream>
#include <unordered_map>

#include "../definitions.h"
#include "../log.hpp"
#include "../ns.h"

//--! 一个req要么
CUB_NS_BEGIN

namespace ctp {
namespace js = rapidjson;

#define LOG_ERROR_AND_RET( _rsp_, _req_, _lst_ )                                                                     \
    do {                                                                                                             \
        if ( pRspInfo->ErrorID != 0 ) {                                                                              \
            LOG_INFO( "ctp error: code=%d msg=%s, req=%d, last=%d", _rsp_->ErrorID, _rsp_->ErrorMsg, _req_, _lst_ ); \
            return;                                                                                                  \
        }                                                                                                            \
    } while ( 0 )

#define CTP_COPY_SAFE( _field_, _str_ ) memcpy( _field_, _str_, std::min( ( int )strlen( _str_ ), ( int )sizeof( _field_ ) - 1 ) )

struct Synchrony {
    using seglist_t = std::list<void*>;

    struct seg_t {
        ~seg_t() {
            for ( auto& v : data ) {
                delete[] v;
            }
        }

        seg_t( seg_t&& s_ ) {
            delete_this();
            data.assign( s_.data );
            s_.data.clear();
        }

        void append( void* data_ ) {
            data.push_back( data_ );
        }

        int size() {
            return ( int )data.size();
        }

        seg_t& operator=( seg_t&& s_ ) {
            delete_this();
            data.assign( s_.data );
            s_.data.clear();

            return *this;
        }

        seg_t& operator=( const seg_t& s_ ) {
            assert( 0 );

            return *this;
        }

        seglist_t data;
    };

    struct entry_t {
        volatile bool           finish = false;
        seg_t                   segments;
        std::mutex              mutex;
        std::condition_variable cv;
    };

    static Synchrony& get();

    void update( int id_, const void* data_, size_t size_, bool finish_ );
    template <typename OBJPTR, typename FUNC, typename... ARGS>
    seg_t wait( int id_, uint32_t timeout_ms_, callback_t cb_, OBJPTR o_, FUNC f_, ARGS&&... a_ ) {
        auto&    sync = Synchrony::get();
        entry_t* e    = sync.put( id_ );
        if ( !e ) return seg_t();

        int rc = ( o_->*f_ )( std::forward<ARGS>( a_ )... );
        if ( rc ) return seg_t();

        std::unique_lock<std::mutex> lock( e->mutex );

        seg_t rc = std::move( e->segments );

        if ( e->finish ) {
            sync.erase( id_ );
            return rc;
        }

        if ( std::cv_status::timeout == e->cv.wait_for( lock, std::chrono::milliseconds( timeout_ms_ ), [ = ]() { return e->finish; } );
             &&!e->mutex.try_lock() ) {
            e->mutex.lock();

            rc = seg_t();
        }

        sync.erase( id_ );

        return rc;
    }

private:
    void     erase( int id_ );
    entry_t* put( int id_ );
    entry_t* fetch( int id_ );

private:
    std::mutex                       _mutex;
    std::unordered_map<int, entry_t> _data;
};

#define CTP_SYNC Synchrony::get()

struct cert_t {
    string_t auth;
    string_t appid;
    string_t token;
};

struct investor_t {
    string_t broker;
    string_t name;
    string_t password;
    string_t id;
};

struct setting_t {
    std::vector<string_t> frontend;
    string_t              flow_path; /* 根据ctp手册，默认值“”表示当前目录*/
    investor_t            i;
    cert_t                c;

    int load( const char* file_ );
};

#define EX_SHFE "SHFE"
#define EX_DCE "DCE"
#define EX_FFEX "FFEX"
#define EX_CZCE "CZCE"
#define EX_INE "INE"
#define EX_GFEX "GFEX"

enum class extype_t : uint8_t {
    SHFE = 0,
    DCE,
    CZCE,
    FFEX,
    INE,
    GFEX,

    Count
};

enum class act_t : uint8_t {
    login,
    logout,
    sub,
    unsub,
    auth,

    put_order,
    cancel_order,
    qry_settle,
    qry_commission,
    qry_fund,
    qry_position
};

/*重复也不太有关系*/
inline int req_id() {
    static std::atomic<int> r = 0;
    return r++;
}

using req_map_t = std::unordered_map<act_t, int>;

inline int cvt_ex( const TThostFtdcExchangeIDType& exid_ ) {
    // LOG_INFO( "ex id=%s", exid_ );
    //  gcc -o2的memcpm是非常快的,常用的交易所放在前面
    if ( memcmp( exid_, EX_SHFE, 4 ) == 0 )
        return ( int )extype_t::SHFE;
    else if ( memcmp( exid_, EX_DCE, 3 ) == 0 )
        return ( int )extype_t::DCE;
    else if ( memcmp( exid_, EX_CZCE, 4 ) == 0 )
        return ( int )extype_t::CZCE;
    else if ( memcmp( exid_, EX_INE, 3 ) == 0 )
        return ( int )extype_t::INE;
    else if ( memcmp( exid_, EX_GFEX, 4 ) == 0 )
        return ( int )extype_t::GFEX;

    LOG_INFO( "cannot reg ex" );
    return -1;
}

inline int setting_t::load( const char* file_ ) {
    std::ifstream json( file_, std::ios::in );

    if ( !json.is_open() ) {
        LOG_INFO( "open setting failed %s", file_ );
        return -1;
    }

    std::stringstream json_stream;
    json_stream << json.rdbuf();
    std::string setting_str = json_stream.str();

    js::Document d;
    d.Parse( setting_str.data() );

    if ( !d.HasMember( "proxies" ) || !d[ "proxies" ].IsArray() ) {
        LOG_INFO( "setting foramt error ,'proxies' node is not array" );
        return -2;
    }

    auto proxies = d[ "proxies" ].GetArray();
    //! 找到第一个enable的就结束
    for ( auto& p : proxies ) {
        if ( !p.HasMember( "enabled" ) || ( p.HasMember( "enabled" ) && !p[ "enabled" ].GetBool() ) ) {
            continue;
        }

        flow_path  = p.HasMember( "flow_path" ) ? p[ "flow_path" ].GetString() : ".";
        i.broker   = p.HasMember( "broker" ) ? p[ "broker" ].GetString() : "uknown";
        i.password = p.HasMember( "password" ) ? p[ "password" ].GetString() : "";
        i.name     = p.HasMember( "user_name" ) ? p[ "user_name" ].GetString() : "";
        frontend.push_back( p.HasMember( "frontend" ) ? p[ "frontend" ].GetString() : "" );

        if ( p.HasMember( "authorization" ) && p[ "authorization" ].IsObject() ) {
            auto& a = p[ "authorization" ];
            c.appid = a.HasMember( "appid" ) ? a[ "appid" ].GetString() : "";
            c.auth  = a.HasMember( "auth_code" ) ? a[ "auth_code" ].GetString() : "";
            c.token = a.HasMember( "token" ) ? a[ "token" ].GetString() : "";
        }

        break;
    }

    // dump
    LOG_TAGGED( "ctp",
                "settings:flow_path=%s,broker=%s, user=%s, pwd=%s, front=%s",
                flow_path.c_str(),
                i.broker.c_str(),
                i.name.c_str(),
                i.password.c_str(),
                frontend[ 0 ].c_str() );

    LOG_TAGGED( "ctp",
                "auth: appid=%s,authcode=%s, token=%s",
                c.appid.c_str(),
                c.auth.c_str(),
                c.token.c_str() );

    return 0;
}

}  // namespace ctp
CUB_NS_END
#endif /* B43732C7_EA9D_4138_8023_E0627CD66A48 */
