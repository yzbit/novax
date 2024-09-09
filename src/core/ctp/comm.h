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

#ifndef B43732C7_EA9D_4138_8023_E0627CD66A48
#define B43732C7_EA9D_4138_8023_E0627CD66A48
#include <algorithm>
#include <atomic>
#include <fstream>
#include <functional>
#include <math.h>
#include <memory>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <sstream>

#include "../calendar.h"
#include "../definitions.h"
#include "../log.hpp"
#include "../ns.h"
#include "synchrony.hpp"

//--! 一个req要么
NVX_NS_BEGIN

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

struct cert_t {
    xstring auth;
    xstring appid;
    xstring token;
};

struct investor_t {
    xstring broker;
    xstring name;
    xstring password;
    xstring id;
};

struct setting {
    std::vector<xstring> frontend;
    xstring              flow_path; /* 根据ctp手册，默认值“”表示当前目录*/
    investor_t           i;
    cert_t               c;

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

inline int setting::load( const char* file_ ) {
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
    LOG_INFO( "settings:flow_path=%s,broker=%s, user=%s, pwd=%s, front=%s",
              flow_path.c_str(),
              i.broker.c_str(),
              i.name.c_str(),
              i.password.c_str(),
              frontend[ 0 ].c_str() );

    LOG_INFO( "ctp",
              "auth: appid=%s,authcode=%s, token=%s",
              c.appid.c_str(),
              c.auth.c_str(),
              c.token.c_str() );

    return 0;
}

inline bool in_trade_time() {
    if ( !CAL.is_trade_day() ) return false;

    auto dt = datetime::now();

    // OnRspAuthenticate 接口返回的“CTP:还没有初始化, 这个错误是什么意思”
    // 此时ctp系统还没有初始化完成，可以简单的理解为系统还没有准备好。期货公司柜台系统一般会在早、晚8点之前准备好，各位投资者的程序可以这个时间点之后启动，没有必要太早启动程序，不同期货公司启动时间不太一致

    // 15:30->20:30
    // 3->8:30
    auto n = dt.t.hour * 100 + dt.t.minute;
    return !( ( n >= 1530 && n <= 2030 ) || ( n >= 300 && n <= 830 ) );
}
}  // namespace ctp

NVX_NS_END  // namespace ctp NVX_NS_END #endif /* B43732C7_EA9D_4138_8023_E0627CD66A48 */
#endif