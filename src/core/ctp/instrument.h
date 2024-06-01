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

#ifndef E5DD7BA7_7534_4689_8CB5_499D80E9EF63
#define E5DD7BA7_7534_4689_8CB5_499D80E9EF63

#include <map>
#include <set>
#include <string>
#include <vector>

#include "../clock.h"
#include "../definitions.h"
#include "../ns.h"
#include "comm.h"

NVX_NS_BEGIN
/*
上一天的主合约可以保存在一个文件中，每天开盘的时候先直接加载，当天更新了以后就重新写入文件
[M1] [M2] [M3] [M4]...
[M1] [M2] [m3] [M4]...更新了M3成m3
*/

#define MAX_CONTRACTS_CNT 500
#define INS_DEF_FILE "fu_main.csv"
#define MAX_CODE_COUNT 256

#define INS_CNT 12

class MonthCode {
public:
    std::string _ins;  // pp OI
    int         _year;
    int         _month;
    std::string _code;
    double      _opi;
    char        _inter;  // 中断标志

    MonthCode() {
        this->clear();
    }

    MonthCode( std::string& code ) {
        this->parse( code );
    }

    void clear() {
        _ins   = "";
        _year  = 0;
        _month = 0;
        _code  = "";
        _opi   = 0.0;
        _inter = KINTER_BROKEN;
    }

public:
    void parse( std::string& str ) {
        _opi  = 0.0;
        _code = "";

        char        a[ 16 ];
        int         ia  = 0;
        const char* ptr = str.c_str();

        this->_code = str;
        // pp10
        while ( *ptr ) {
            a[ ia++ ] = *ptr;
            if ( *ptr >= '0' && *ptr <= '9' ) {
                a[ ia ]    = '\0';
                this->_ins = a;
                break;
            }
            ptr++;
        }  //

        int ym       = atoi( ptr );
        this->_month = ym % 100;
        this->_year  = ym / 100;
        // pp1909
        if ( strlen( ptr ) == 4 ) {
            this->_year = 2000 + this->_year;
        }
        else {
            // ap901 ap001
            this->_year = 0;  // 太难算了，没什么必要
        }
    }
};

class InstrumentCode {
public:
    std::string _ins;
    // 从month开始有效的第一个合约，包括month本身
    // month=1-->1
    // month=12->12
    //
    MonthCode& next( int month ) {
        // month只能是1-12，如果传进来13，那依然是1；
        if ( month > INS_CNT + 1 ) {
            TraceWarn( "[MonthCode->next] 超出范围 {:d}, 取 - 1\n", month );
            month = 1;
        }

        if ( month == INS_CNT + 1 ) month = 1;

        month -= 1;
        int loop = month;
        while ( true ) {
            if ( _all[ loop ]._code.empty() ) {
                loop = ( loop + 1 ) % INS_CNT;
            }
            else {
                return _all[ loop ];
            }

            // loop back
            if ( ( month == 0 && loop == INS_CNT - 1 ) || ( month > 0 && loop == month - 1 ) ) {
                break;
            }

        }  // while

        return _all[ loop ];
    }

    std::vector<MonthCode> _all;
};

// 只交易流动性最好的1个或者2个合约
struct MainInsInfo {
    std::string _ins;  // 合约代码，比如rb，pp
    MonthCode   _main;
    MonthCode   _sub;
    int         _update;

    bool toshift() {
        return _main._opi < _sub._opi && _update != lutils::getYMD();
    }

    MainInsInfo() {
        _ins    = "";
        _update = 0;
    }
};
// 开始订阅合约的行情数据--
// 默认订阅所有合约（定义在配置文件中）
// 每天开盘的时候先订阅所有的合约，然后比较一下哪个合约的opi最大，选择那个作为主要的合约----这里很trick
// 一建立连接和登录以后，立刻进行这个操作。筛选算法参照LqMarketData2
int LqCtpProxy::Subcribe() {
    std::vector<std::string> c = {};
    for ( auto& m : _MainCodes ) {
        c.push_back( m.second._main._code );
        if ( !m.second._sub._code.empty() ) {
            c.push_back( m.second._sub._code );
        }
    }

    TraceInfo( "subscribe {} instrument codes\n", c.size() );
    return this->Subcribe( c );
}

int LqCtpProxy::Subcribe( std::vector<std::string>& codes ) {
    char* ins[ MAX_CONTRACTS_CNT ];
    char  tmp[ 500 ][ 7 ];
    int   i = 0;

    TraceInfo( "\n----订阅------>|\n" );
    for ( auto& m : codes ) {
        strcpy_s( tmp[ i ], 7, m.c_str() );
        ins[ i ] = tmp[ i ];
        i++;
        TraceInfo( "*{}", ins[ i - 1 ] );
    }
    int count = ( int )codes.size();
    TraceInfo( "\n[ {:d} ]|<----------\n\n", count );
    return this->svc.xSubMarketData( ins, count );
}

struct Instrument {
    Instrument();
    nvx_st load();

private:
    nvx_st   load( const char* file_ );
    int      process();
    nvx_st   process_line( xstring& line );
    void     process_code( xstring* codearr, std::vector<std::string>& out );
    xstring process_code( xstring& c, bool y );
    void     trim( xstring& str_ );
    void     split( const xstring& src_, char deli_, std::vector<xstring>& out_ );

private:
    std::vectror<code> _codes;
#if 0
    xstring                           _file;
    std::vector<xstring>              lines;
    std::map<xstring, InstrumentCode> _codes;
    int                                total;
#endif
};

NVX_NS_END

#endif /* E5DD7BA7_7534_4689_8CB5_499D80E9EF63 */
