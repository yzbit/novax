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
