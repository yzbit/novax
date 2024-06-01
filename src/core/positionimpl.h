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

#ifndef BA6DB31D_0B8F_4679_B2F6_B9726A037824
#define BA6DB31D_0B8F_4679_B2F6_B9726A037824
#include <novax.h>

NVX_NS_BEGIN

struct position_impl : position {
    position_impl( const code& code_, bool long_ );
    bool        is_short() const { return _long; }
    const code& symbol() const { return _p.symbol; }
    vol         herge( vol qty_, price price_ );
    void        accum( vol qty_, price price_ );
    void        reset();

    nvx_st stop( vol qty_, price price_, stop_mode t_ ) override;
    nvx_st gain( vol qty_, price price_ ) override;
    price  avg_dealt() override;
    vol    qty() override;
    nvx_st close() override { return 0; }
    // kidx   last_entry() override;
    // kidx   last_exit() override;

private:
    position_impl() = delete;

private:
    bool     _long;
    pos_item _p;
};

NVX_NS_END

#endif /* BA6DB31D_0B8F_4679_B2F6_B9726A037824 */
