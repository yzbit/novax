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

#include <stdio.h>
#include <vector>

struct IndicatorBase {
    virtual void calc() = 0;
};

template <typename T>
struct Indicator : IndicatorBase {
    virtual ~Indicator() {
    }

    void calc() override {
        // T::calc();
        printf( "indicator calc\n" );
    }
};

#if 0
struct MaIndicator : Indicator<MaIndicator> {
    static void calc() {
        printf( "ma calculate" );
    }
};
#endif

int main() {
    // std::vector<IndicatorBase*> v;

    IndicatorBase* base = new Indicator<int>();
    base->calc();

#if 0

    Indicator<MaIndicator>* b = new MaIndicator();
    // v.emplace_back( static_cast<Indicator<void>*>( b ) );
    v.emplace_back( dynamic_cast<Indicator<void>*>( b ) );

    // b->calc();
    v[ 0 ]->calc();
#endif

    return 0;
}

#if 0
struct RollIndicator : Indicator<RollIndicator> {
    void calc()
        printf( "ma calculate" );
};

    // Indicator<void>* b = new MaIndicator();
#endif
