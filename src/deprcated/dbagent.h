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

#ifndef E16389B7_2FB3_4F8D_9784_6CEABEA06C08
#define E16389B7_2FB3_4F8D_9784_6CEABEA06C08
#include <functional>
#include <stdint.h>

#include "definitions.h"
#include "ns.h"

NVX_NS_BEGIN
struct db_agent {
    enum class dbtype_t {
        SQLite,
        MySQL
    };

    struct param_t {
        xstring server = "";
        xstring dbname = "";
        xstring user   = "";
        xstring pwd    = "";
        bool    create = true;
        void*   data   = nullptr;
    };

    using dbcallback_t = std::function<int( void* usrdata, int colcnt, char** colvalue, char** colname )>;

    using conn_t = void*;
    using id_t   = int64_t;

    static db_agent* create( dbtype_t dt_ );

    virtual conn_t open( const char* url_, const char* usr_, const char* pwd_, const char* dbname_ = nullptr, bool create_ = true, void* param_ = nullptr ) = 0;
    virtual conn_t open( param_t& p_ );
    virtual conn_t conn() { return _conn; }
    virtual nvx_st close() = 0;
    virtual bool   is_open() const;
    virtual nvx_st transaction() { return NVX_FAIL; }
    virtual nvx_st commit() { return NVX_FAIL; }
    virtual nvx_st rollback() { return NVX_FAIL; }
    virtual nvx_st execute( const xstring& clause_ ) = 0;
    virtual id_t   last_key( const char* table_ )    = 0;
    virtual nvx_st fetch( const xstring& clause_, dbcallback_t cb_, void* p1 = nullptr, void* p2 = nullptr ) { return NVX_FAIL; };

protected:
    db_agent();
    virtual ~db_agent();

protected:
    conn_t _conn = nullptr;
};

NVX_NS_END

#endif /* E16389B7_2FB3_4F8D_9784_6CEABEA06C08 */
