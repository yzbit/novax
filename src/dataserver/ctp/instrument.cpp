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

#include <cctype>
#include <fstream>
#include <iostream>
#include <vector>

#include "instruments.h"

Instrument::Instrument() {
    _codes.reserve( MAX_CODE_COUNT );
}

int Instrument::process() {
    for ( auto& s : this->lines ) {
        this->processLine( s );
    }

    return ( int )_codes.size();
}

// 整数字符串yyyymm拆成年份yyyy和月份mm
void Instrument::getYearMonth( int& year, int& mon ) {

    year = mon = lutils::dayArr2Int( lutils::getDate().c_str() );

    mon = ( mon / 100 ) % 100;

    if ( mon > 12 || mon < 1 ) {
        mon = -1;  // invalid.
    }

    year = year / 10000;
}

//
// 把codearr->CUXX01中的x换成年份，比如1901或者901
// codearr是三个元素的数组，我们每次从当前行中提取三个合约，其中有一个肯定是主力
// 结果全部存在out中，会进行订阅然后筛选真正的主力
std::string Instrument::process_code( std::string& c, bool nextyear ) {
    int y, m;
    this->getYearMonth( y, m );

    if ( nextyear ) {
        y++;
    }

    std::string r = "";
    // if --XX--
    if ( c.find( "XX" ) != std::string::npos ) {
        r = lutils::sformat( "%02d", y % 100 );  // 20--19
        c = lutils::replace( c, std::string( "XX" ), r );
    }
    else if ( c.find( "X" ) != std::string::npos ) {
        r = lutils::sformat( "{:d}", y % 10 );  // 20-1-9
        c = lutils::replace( c, std::string( "X" ), r );
    }
    else {
        // nothing
    }

    return c;
}

void Instrument::split( const xstring& src_, char deli_, std::vector<xstring>& out_ ) {
    if ( !std::isprint( deli_ ) ) {
        out_.emplace_back( src_ );
    }

    int start = 0;
    for ( int i = 0; i < str.size(); i++ ) {
        if ( str[ i ] == delim ) {
            res.push_back( str.substr( start, i - start ) );
            start = i + 1;
        }
    }
    res.push_back( str.substr( start ) );
}

void Instrument::trim( xstring& str_ ) {
    str_.erase( str.find_last_not_of( " \t\n\r\f\v" ) + 1 );
    str_.erase( 0, str.find_first_not_of( " \t\n\r\f\v" ) );
}

// 处理合约配置文件中的一行
void Instrument::process_line( std::string& line_ ) {
    trim( line_ );
    if ( line.empty() )
        return;

    std::vector<xstring> out;
    out.reserve( 16 );

    // 以#或者//开头或者以非字母开头的都当作注释或者垃圾行抛弃
    if ( line_.empty() || line_.at( 0 ) == '#' || ( line_.size() > 1 && line_.at( 0 ) == '/' && line_.at( 1 ) == '/' ) ) {
        return;
    }

    split( line_, ",", out );

    // 1个序号+12个月份
    if ( out.size() != 13 ) {
        LOG_INFO( "Instrument::processLine >> incorrect line", line_.c_str() );
        return;
    }
    out.erase( out.begin() );  // skip 07 prefix

    auto dt    = datetime::now();
    int  year  = dt.day.year;
    int  month = dt.day.month;

    // 每一行的格式如下：
    /*07, ------, ------, ------, ------, ------, AUXX06, ------, ------, ------, ------, ------, AUXX12*/

    // 对于很多合约来说，当主力合约是1909合约的时候，下一年的2001合约还没生成，此时订阅2001会出问题，但是无论如何主力合约肯定没错
    InstrumentCode ic      = {};
    std::string    inscode = "";
    for ( int loop = 0; loop < 12; loop++ ) {
        std::string code = this->processCode( sout[ loop ], loop < month );
        MonthCode   mc   = {};
        // mc._ins = Instrument::ins_code(code);
        mc._month = loop + 1;
        mc._year  = loop < month ? ( year + 1 ) : year;
        if ( code.find( "-" ) != std::string::npos ) {
            mc._code = "";
        }
        else {
            mc._code = code;
            inscode  = Instrument::ins_code( code );
        }

        ic._all.push_back( mc );
        if ( ic._ins.empty() ) {
            ic._ins = inscode;
        }
    }

    if ( _codes.find( ic._ins ) == _codes.end() ) {
        _codes.insert( make_pair( ic._ins, ic ) );
    }
}

nvx_st Instrument::load( const char* file_ ) {
    std::ifstream ifs( f, std::ios::in );

    if ( !ifs.is_open() ) {
        LOG_INFO( "open ins file failed: %s", file_ );
        return NVX_FAIL;
    }

    std::string line;

    while ( !ifs.eof() ) {
        getline( ifs, line );
        process_line( line );
    }

    ifs.close();
    /// TraceInfo( "\n代码文件加载完毕，一共 {:d} 行,开始处理代码文件\n", ( int )lines.size() );

    return NVX_OK;
}

nvx_st Instrument::load() {
    return load( INS_DEF_FILE );
}
