#ifndef B0FD204B_DBB0_4DB4_BA81_807E439AA053
#define B0FD204B_DBB0_4DB4_BA81_807E439AA053
#include <chrono>

#include "../definitions.h"
#include "../indicator.h"
#include "../ns.h"

CUB_NS_BEGIN

struct Kline : Indicator {
    Kline( const code_t& code_, const period_t& p_, int series_count_ );
    static Kline* create( const arg_pack_t& arg_ );

    void on_init() override;
    void on_calc( const quotation_t& q_ ) override;

private:
    int      _count;
    code_t   _symbol;
    period_t _period;
    int      _curr_bar;
};

CUB_NS_END

#if 0
bool hammer() {
        if ( bodysize() < lowers() * 0.5
             && uppers() < bodysize() ) {

            if ( kutils::AVG_SHADOW_SIZE > 0 && uppers() > kutils::AVG_SHADOW_SIZE ) {
                return false;
            }

            return true;
        }

        return false;
    }

    bool rhammer() {
        if ( bodysize() < uppers() * 0.5
             && lowers() < bodysize() ) {

            if ( kutils::AVG_SHADOW_SIZE > 0 && lowers() > kutils::AVG_SHADOW_SIZE ) {
                return false;
            }

            return true;
        }
        return false;
    }

    bool closetop() {
        if ( 0 == barsize() ) {
            return false;
        }

        return C >= O && ( H - C ) / barsize() < kutils::TINY_PERCENT;
    }

    bool closebtm() {
        if ( 0 == barsize() ) {
            return false;
        }

        return ( C <= O ) && ( C - L ) / barsize() < kutils::TINY_PERCENT;
    }

    bool redmarubozu() {
        return C == H && O == L && bodysize() > kutils::AVG_BODY_SIZE;
    }

    bool blackmarubozu() {
        return C == L && O == L && bodysize() > kutils::AVG_BODY_SIZE;
    }

    //地步光头长阳，这是一个潜在的支撑位
    bool xredmarubozu() {
        return bodysize() > kutils::AVG_BODY_SIZE && O == L && closetop();
    }

    bool xblackmarubozu() {
        return bodysize() > kutils::AVG_BODY_SIZE && O == H && closebtm();
    }

    bool weak() {
        return barsize() < kutils::AVG_BODY_SIZE && bodysize() < kutils::AVG_BODY_SIZE && vol() < kutils::AVG_VOL_SIZE;
    }
#endif

#endif /* B0FD204B_DBB0_4DB4_BA81_807E439AA053 */
