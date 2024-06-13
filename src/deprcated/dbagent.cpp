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

#include <sqlite/sqlite3.h>

#include "dbagent.h"

#include "log.hpp"

NVX_NS_BEGIN

struct sqlite_agent : db_agent {
private:
    conn_t open( const char* url_, const char* usr_, const char* pwd_, const char* dbname_ = nullptr, bool create_ = true, void* param_ = nullptr ) override;
    nvx_st close() override;
    nvx_st transaction() override;
    nvx_st commit() override;
    nvx_st rollback() override;
    nvx_st execute( const xstring& sql_ ) override;
    nvx_st fetch( const xstring& clause_, dbcallback_t cb_, void* p1, void* p2 ) override;
    id_t   last_key( const char* table_ ) override;
};

db_agent* db_agent::create( db_agent::dbtype_t at ) {
    switch ( at ) {
    default:
        return nullptr;
    case db_agent::dbtype_t::SQLite:
        return new sqlite_agent();
    }

    return nullptr;
}

db_agent::conn_t db_agent::open( db_agent::param_t& p ) {
    return open( p.server.c_str(), p.user.c_str(), p.pwd.c_str(), p.dbname.c_str(), p.create, p.data );
}

bool db_agent::is_open() const {
    return _conn != nullptr;
}

db_agent::conn_t sqlite_agent::open( const char* url, const char* usr, const char* pwd, const char* dbname, bool create, void* param ) {
    sqlite3* sql    = nullptr;
    int      result = SQLITE_OK;

    if ( is_nil( url ) && is_nil( dbname ) )
        return nullptr;

    xstring dbpath = "";
    if ( is_nil( url ) )
        dbpath = dbname;
    else if ( is_nil( dbname ) )
        dbpath = url;
    else
        dbpath = xstring( url ) + xstring( dbpath );

    if ( create )
        result = sqlite3_open_v2( dbpath.c_str(), &sql, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_SHAREDCACHE, NULL );
    else
        result = sqlite3_open_v2( dbpath.c_str(), &sql, SQLITE_OPEN_READWRITE | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_SHAREDCACHE, NULL );

    if ( result == SQLITE_OK ) {
        LOG_INFO( "create db ok=%s\n", dbpath );
    }
    else {
        LOG_INFO( "create db fail={%d}\n", result );
    }

    _conn = sql;

    return sql;
}

nvx_st sqlite_agent::close() {
    sqlite3* sql = ( sqlite3* )conn();
    sqlite3_close_v2( sql );
    _conn = nullptr;

    return NVX_OK;
}

nvx_st sqlite_agent::transaction() {
    sqlite3* sql = ( sqlite3* )conn();
    if ( !sql ) return NVX_FAIL;

    return sqlite3_exec( sql, "BEGIN;", nullptr, nullptr, nullptr ) ? NVX_FAIL : NVX_OK;
}

nvx_st sqlite_agent::commit() {
    sqlite3* sql = ( sqlite3* )conn();
    if ( !sql ) return NVX_FAIL;

    return sqlite3_exec( sql, "COMMIT;", nullptr, nullptr, nullptr ) ? NVX_FAIL : NVX_OK;
}

nvx_st sqlite_agent::rollback() {
    sqlite3* sql = ( sqlite3* )conn();
    if ( !sql ) return NVX_FAIL;

    return sqlite3_exec( sql, "ROLLBACK;", nullptr, nullptr, nullptr ) ? NVX_FAIL : NVX_OK;
}

nvx_st sqlite_agent::execute( const xstring& clause_ ) {
    sqlite3* sql = ( sqlite3* )conn();
    if ( !sql ) return NVX_FAIL;

#if 0
	sqlite3_stmt* stmt = NULL;

	const char* csql = clause.c_str();
	int result = sqlite3_prepare_v2(sql, csql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		result = sqlite3_step(stmt);

		if (result != SQLITE_DONE &&result != SQLITE_ROW && result != SQLITE_OK) {
			//VMS_ERR("Execute[1]@锟斤拷锟捷匡拷锟斤拷锟斤拷执锟叫筹拷锟斤拷,锟斤拷锟斤拷锟斤拷锟�,code={%d},sql={%s}\n", result, clause.c_str());
			return VMS_FAIL;
		}
	}
	else {
		//VMS_ERR("Execute[2]@锟斤拷锟捷匡拷锟斤拷锟斤拷执锟叫筹拷锟斤拷,锟斤拷锟斤拷锟斤拷锟�,code={%d},sql={%s}\n",result,clause.c_str());
		return VMS_FAIL;
	}

	result = sqlite3_finalize(stmt);
	if (result != SQLITE_OK) {
		//VMS_ERR("Execute[3]@锟斤拷锟捷匡拷锟斤拷锟斤拷执锟叫筹拷锟斤拷,锟斤拷锟斤拷锟斤拷锟�,code={%d},sql={%s}\n", result, clause.c_str());
		return VMS_FAIL;
	}

	return VMS_OK;
#endif
    return sqlite3_exec( sql, clause_.c_str(), nullptr, nullptr, nullptr ) ? NVX_FAIL : NVX_OK;
}

nvx_st sqlite_agent::fetch( const xstring& clause_, dbcallback_t cb_, void* p1, void* p2 ) {
    sqlite3* sql = ( sqlite3* )conn();
    if ( !sql ) return NVX_FAIL;

    return sqlite3_exec( sql, clause_.c_str(), cb_.target<int( void*, int, char**, char** )>(), p1, ( char** )p2 ) ? NVX_FAIL : NVX_OK;
}

db_agent::id_t sqlite_agent::last_key( const char* table ) {
    sqlite3* sql = ( sqlite3* )conn();
    return sqlite3_last_insert_rowid( sql );
}

NVX_NS_END