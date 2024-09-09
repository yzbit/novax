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

* \author: yaozn(zinan@outlook.com), qianq(695997058@qq.com)
* \date: 2024
**********************************************************************************/

#include <filesystem>

#include "mdproxy.h"

#include "../log.hpp"
#include "../models.h"
#include "comm.h"
// todo debug
#include "../utils.hpp"

#define CTP_CHECK_READY( _r_ )              \
    do {                                    \
        if ( !_api ) {                      \
            LOG_INFO( "ctp not initiaed" ); \
            return _r_;                     \
        }                                   \
    } while ( 0 )

NVX_NS_BEGIN
namespace ctp {
namespace fs = std::filesystem;

mdex::mdex( ipub* p_ )
    : market( p_ ) {
}

nvx_st mdex::subscribe( const code& code_ ) {
    std::unique_lock<std::mutex> lock{ _sub_mtx };

    LOG_INFO( "ctp",
                "subscribe svc=%d, code=%s, pending sub=%d pending_unsub=%d",
                _is_svc_online,
                code_.c_str(),
                ( int )_sub_symbols.size(),
                ( int )_unsub_symbols.size() );

    _sub_symbols.insert( code_ );
    _unsub_symbols.erase( code_ );

    if ( _is_svc_online ) {
        sub( const_cast<code&>( code_ ) );
    }

    return NVX_OK;
}

nvx_st mdex::unsubscribe( const code& code_ ) {
    std::unique_lock<std::mutex> lock{ _sub_mtx };

    LOG_INFO( "ctp",
                "UNsubscribe svc=%d, code=%s, pending sub=%d pending_unsub=%d",
                _is_svc_online,
                code_.c_str(),
                ( int )_sub_symbols.size(),
                ( int )_unsub_symbols.size() );

    if ( _sub_symbols.count( code_ ) == 0 ) return NVX_OK;

    _sub_symbols.erase( code_ );
    _unsub_symbols.insert( code_ );

    if ( _is_svc_online ) {
        unsub( const_cast<code&>( code_ ) );
    }

    return NVX_OK;
}

nvx_st mdex::sub() {
    CTP_CHECK_READY( NVX_FAIL );

    if ( _sub_symbols.empty() )
        return NVX_OK;

    auto arr = set2arr( _sub_symbols );

    return _api->SubscribeMarketData( arr.get(), _sub_symbols.size() );
}

nvx_st mdex::unsub() {
    CTP_CHECK_READY( NVX_FAIL );

    if ( _sub_symbols.empty() ) return NVX_OK;

    auto arr = set2arr( _sub_symbols );
    return _api->UnSubscribeMarketData( arr.get(), _unsub_symbols.size() );
}

std::unique_ptr<char*[]> mdex::set2arr( std::set<code>& s ) {
    auto arr = std::make_unique<char*[]>( _sub_symbols.size() );
    int  n   = 0;

    for ( auto& s : s ) {
        arr[ n++ ] = s.c_str();
        // fprintf( stderr, "#### %s\n", arr[ n - 1 ] );
    }

    return arr;
}

nvx_st mdex::sub( code& code_ ) {
    CTP_CHECK_READY( NVX_FAIL );

    char* c = code_.c_str();
    return _api->SubscribeMarketData( &c, 1 );
}

nvx_st mdex::unsub( code& code_ ) {
    CTP_CHECK_READY( NVX_FAIL );

    char* c = code_.c_str();
    return _api->UnSubscribeMarketData( &c, 1 );
}

id_t mdex::session_id() {
    static id_t _rid = 0;
    return ++_rid;
}

nvx_st mdex::login() {
    CThostFtdcReqUserLoginField login_req = { 0 };

    strncpy( login_req.BrokerID, _settings.i.broker.c_str(), sizeof( login_req.BrokerID ) );
    strncpy( login_req.UserID, _settings.i.name.c_str(), sizeof( login_req.UserID ) );
    strncpy( login_req.Password, _settings.i.password.c_str(), sizeof( login_req.Password ) );

    auto session = session_id();

    int rt = this->_api->ReqUserLogin( &login_req, session );
    if ( !rt ) {
        LOG_INFO( "i> LqSvcMarket::xLogin sucessfully, sess=%u\n", ( unsigned )session );
        return NVX_OK;
    }
    else {
        LOG_INFO( "c> LqSvcMarket::xLogin failed\n" );
        return NVX_FAIL;
    }

    return 0;
}

nvx_st mdex::start() {
    LOG_INFO( "ctp", "start to run ,state=%d", _running );
    if ( _running ) return NVX_OK;
    _running = true;

    return init();
}

nvx_st mdex::stop() {
    LOG_INFO( "ctp", "stop running ,[do NOTHING] state=%d", _running );
    return NVX_OK;
}

nvx_st mdex::init() {
    LOG_INFO( "ctp", "init begin" );

    if ( _settings.load( CTP_MD_SETTING_FILE ) < 0 ) {
        LOG_INFO( "#ERR,read ctp setings failed" );
        return NVX_FAIL;
    }

    LOG_INFO( "ctp md init,flow=%s,font=%s", _settings.flow_path.c_str(), _settings.frontend[ 0 ].c_str() );
    std::thread( [ & ]() {
        _api = CThostFtdcMdApi::CreateFtdcMdApi( _settings.flow_path.c_str(), false );  // true: udp mode
        _api->RegisterSpi( this );

        for ( auto& f : _settings.frontend ) {
            _api->RegisterFront( const_cast<char*>( f.c_str() ) );
        }

        _api->Init();

        LOG_INFO( "spawn md user" );
        auto rc = _api->Join();
        LOG_INFO( "ctp", "md api exit with code=%d", rc );
    } ).detach();

    return NVX_OK;
}

// ctp overrides
void mdex::OnFrontConnected() {
    LOG_INFO( "ctp ex md connected ,begin to login" );
    login();
}

void mdex::OnFrontDisconnected( int nReason ) {
    LOG_INFO( "ctp ex md disconnected,reason=%d", nReason );

    _is_svc_online = false;
}

//! as doc: this is decrecated
void mdex::OnHeartBeatWarning( int nTimeLapse ) {
}

// todo
// #这里有问题，如果重连的时候是否需要重新订阅
void mdex::OnRspUserLogin( CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    LOG_ERROR_AND_RET( pRspInfo, nRequestID, bIsLast );

    LOG_INFO( "@INFO, login:day=%s, time=%s,brokerid=%s, user=%s, sysname=%s,frontid=%s,session=%s, order_ref=%d",
              pRspUserLogin->TradingDay,
              pRspUserLogin->LoginTime,
              pRspUserLogin->BrokerID,
              pRspUserLogin->UserID,
              pRspUserLogin->SystemName,
              pRspUserLogin->FrontID,
              pRspUserLogin->SessionID,
              pRspUserLogin->MaxOrderRef );

    datetime dt = { 0 };

    LOG_INFO( "ctp", "tune clock of exchanges" );
    CTP_CLOCK.reset( pRspUserLogin );

    {
        LOG_INFO( "ctp", "login ok , process subcribtion" );
        for ( auto& c : _sub_symbols ) {
            LOG_INFO( "=> %s", c.c_str() );
        }

        LOG_INFO( "unsubcribtion" );
        for ( auto& c : _unsub_symbols ) {
            LOG_INFO( "=> %s", c.c_str() );
        }

        std::unique_lock<std::mutex> lock{ _sub_mtx };
        unsub();
        sub();

        _is_svc_online = true;
    }
}

void mdex::OnRspUserLogout( CThostFtdcUserLogoutField* pUserLogout, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    LOG_ERROR_AND_RET( pRspInfo, nRequestID, bIsLast );

    LOG_INFO( "user logout: broker=%s ,user=%s, req=%d", pUserLogout->BrokerID, pUserLogout->UserID, nRequestID );
    _is_svc_online = false;
}

void mdex::OnRtnDepthMarketData( CThostFtdcDepthMarketDataField* f ) {
    // fprintf( stderr, "r quote\n" );
    pub::tick_msg_t q;

    q.time.from_ctp( f->TradingDay, f->UpdateTime, f->UpdateMillisec );
    q.ex       = cvt_ex( f->ExchangeID );  // 模拟盘是[\0]
    q.last     = f->LastPrice;
    q.volume   = f->Volume;
    q.code     = f->InstrumentID;
    q.opi      = f->OpenInterest;
    q.ask      = f->AskPrice1;
    q.askvol   = f->AskVolume1;
    q.bid      = f->BidPrice1;
    q.bidvol   = f->BidVolume1;
    q.highest  = f->HighestPrice;
    q.lowest   = f->LowestPrice;
    q.avgprice = f->AveragePrice;
    q.turnover = f->Turnover;
    q.open     = f->OpenPrice;
    q.close    = q.last;  //->ClosePrice;  // 今收盘价格，盘中是一个错误的值，不太有意义  PreClosePrice 昨收盘

    // tick_msg_t m( q );
    PUB( q );

    fprintf( stderr,
             "\tdate=%s time=%s ms=%u ex=%d\n\tlast=%.0lf vol=%d code=%s opi=%u \n\task=%.0lf askv=%u bid=%.0lf bidv=%u higest=%.0lf lowest=%.0lf \n\tavg=%.0lf turnover=%.0lf open=%.0lf close=%d\n",
             f->TradingDay,
             f->UpdateTime,
             f->UpdateMillisec,
             q.ex,
             q.last,
             q.volume,
             q.code.c_str(),
             q.opi,
             q.ask,
             q.askvol,
             q.bid,
             q.bidvol,
             q.highest,
             q.lowest,
             q.avgprice,
             q.turnover,
             q.open,
             ( int )q.close );

    /*
    鉴于夜盘交易时间非常混乱，我们不使用服务器时间（日期），和常用的交易软件时间划分类似
    夜盘算一整天的开始，所以只要时间在下午3点之后都算第二天，9点之后算当天

    但是。。。这会有节假日的问题，这会极大的增加问题的复杂度，我们可以记录两个值来表示日期
    当前日期和偏移量，晚上偏移量是1，白天偏移量是0，但是。。。。新的问题来了，本来这个只是为了数据存储使用（一天一个文件夹）

    实际上这个偏移量也不用记，根据时间推算就可以了--周末和节假日怎么办
    文华财经是按自然日来做的，9点到夜里收盘算是一天

    https://zhuanlan.zhihu.com/p/33553653
    总之（敲黑板），处理上期所和大商所的夜盘品种，可以直接用TradingDay取一个完整的交易日，而处理郑商所的夜盘品种，则需要拼接上一个TradingDay的夜盘和当前TradingDay的日盘作为一个完整的交易日。
    */
}

void mdex::OnRspQryMulticastInstrument( CThostFtdcMulticastInstrumentField* pMulticastInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    LOG_ERROR_AND_RET( pRspInfo, nRequestID, bIsLast );
    LOG_INFO( "multicast return" );
}

void mdex::OnRspError( CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    LOG_INFO( "rsp error,code=%d, msg=%s", pRspInfo->ErrorID, pRspInfo->ErrorMsg );
}

void mdex::OnRspSubMarketData( CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    LOG_ERROR_AND_RET( pRspInfo, nRequestID, bIsLast );

    dumpHex( ( const uint8_t* )( pSpecificInstrument->InstrumentID ), sizeof( pSpecificInstrument->InstrumentID ), sizeof( pSpecificInstrument->InstrumentID ) );
    LOG_INFO( "subscribe return: %s", pSpecificInstrument->InstrumentID );
}

void mdex::OnRspUnSubMarketData( CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    if ( !pSpecificInstrument ) {
        return;
    }

    LOG_ERROR_AND_RET( pRspInfo, nRequestID, bIsLast );

    dumpHex( ( const uint8_t* )( pSpecificInstrument->InstrumentID ), sizeof( pSpecificInstrument->InstrumentID ), sizeof( pSpecificInstrument->InstrumentID ) );
    LOG_INFO( "un subscribe return: %s", pSpecificInstrument->InstrumentID );
}

void mdex::OnRspSubForQuoteRsp( CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    LOG_ERROR_AND_RET( pRspInfo, nRequestID, bIsLast );
    LOG_INFO( "rsp for quote" );
}

void mdex::OnRtnForQuoteRsp( CThostFtdcForQuoteRspField* pForQuoteRsp ) {
    LOG_INFO( "QUOTE RSP" );
}

void mdex::OnRspUnSubForQuoteRsp( CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    LOG_ERROR_AND_RET( pRspInfo, nRequestID, bIsLast );
    LOG_INFO( "rsp for unsubquote" );
}
}  // namespace ctp
NVX_NS_END
