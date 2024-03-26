

#include "CompositeUpdater.h"
#include "Harmony2.h"
#include "TestComposite.h"

#include "asserts.h"

template <class TBase>
class MyComposite : public TBase {
public:
    enum ParamIds {
        PARAM1
    };
    enum InputIds {
        INPUT1
    };
};

using Comp = MyComposite<TestComposite>;

static void testCanCall() {
    Comp composite;
    CompositeUpdater<Comp> c({}, {});
    CVUpdater<Comp> cv(&composite, Comp::PARAM1);
    ParamUpdater<Comp> param(&composite, Comp::INPUT1);
}

void testUpdater() {
    assert(false);
}

#if 1
void testFirst() {
    // i == 0, j== -2
    // testModeCV(-14, 0, true);
    testUpdater();
}
#endif