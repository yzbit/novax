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

#ifndef E93F5C75_9223_409D_8F98_DFFDE2E179BF
#define E93F5C75_9223_409D_8F98_DFFDE2E179BF

#include <config.h>

#include "models.h"
#include "ns.h"

NVX_NS_BEGIN

struct context;
struct strategy {
    virtual ~strategy() {}

    virtual xstring name()                                 = 0;
    virtual nvx_st  on_init( config* cfg_, context* c_ )   = 0;
    virtual nvx_st  on_tick( const tick* q_, context* c_ ) = 0;

    virtual nvx_st on_error( const nvxerr* err_, context* c_ ) { return NVX_OK; }
    virtual nvx_st on_clock( const datetime* dt_, context* c_ ) { return NVX_OK; }
    virtual nvx_st on_order( oid id_, context* c_ ) { return NVX_OK; }
    virtual nvx_st on_instate( const void* par_, context* c_ ) { return NVX_OK; }
};

NVX_NS_END

#endif /* E93F5C75_9223_409D_8F98_DFFDE2E179BF */
