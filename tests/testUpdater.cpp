

#include "CompositeUpdater.h"
#include "Harmony2.h"
#include "TestComposite.h"
#include "asserts.h"

template <class TBase>
class MyComposite : public TBase {
public:
    enum ParamIds {
        PARAM1,
        PARAM2
    };
    enum InputIds {
        INPUT1,
        INPUT2,
        INPUT3
    };
};

using Comp = MyComposite<TestComposite>;

static void testCanCall() {
    Comp composite;
    CompositeUpdater<Comp> c({}, {});
    CVUpdater<Comp> cv(&composite, Comp::PARAM1);
    ParamUpdater<Comp> param(&composite, Comp::INPUT1);

    std::vector<CVUpdater<Comp>> v;
    v.push_back({&composite, int(Comp::PARAM1)});
    v.push_back({&composite, Comp::PARAM1});
    std::vector<ParamUpdater<Comp>> vpu;
    vpu.push_back({&composite, 4});

    std::vector<CVUpdater<Comp>> v2(std::move(v));
    CompositeUpdater<Comp> c2(std::move(vpu), std::move(v));
    CompositeUpdater<Comp> v3(
        {{&composite, 2}},
        {{&composite, 5}});
    const bool b = v3.check();
}

static void testPollParam() {
    Comp composite;
    ParamUpdater pu(&composite, Comp::PARAM1);
    ParamUpdater pu2(&composite, Comp::PARAM2);

    bool b = pu.check();
    assertEQ(b, true);
    b = pu.check();
    assertEQ(b, false);

    b = pu2.check();
    assertEQ(b, true);

    composite.params[Comp::PARAM2].value = 66;

    b = pu.check();
    assertEQ(b, false);
    b = pu2.check();
    assertEQ(b, true);

    b = pu.check();
    assertEQ(b, false);
    b = pu2.check();
    assertEQ(b, false);
}

static void testPollCVChannels() {
    Comp composite;
    CVUpdater cv(&composite, Comp::INPUT1);
    CVUpdater cv2(&composite, Comp::INPUT2);

    bool b = cv.check();
    assertEQ(b, true);
    b = cv2.check();
    assertEQ(b, true);

    b = cv.check();
    assertEQ(b, false);
    b = cv2.check();
    assertEQ(b, false);

    auto& in1 = composite.inputs[Comp::INPUT1];
    auto& in2 = composite.inputs[Comp::INPUT2];

    in2.channels = 5;
    b = cv.check();
    assertEQ(b, false);
    b = cv2.check();
    assertEQ(b, true);      // channels changed
    b = cv2.check();
    assertEQ(b, true);      // check underlying value
      b = cv2.check();
    assertEQ(b, false); 
}

void testUpdater() {
    testCanCall();
    testPollParam();
    testPollCVChannels();
}

#if 1
void testFirst() {
    // i == 0, j== -2
    // testModeCV(-14, 0, true);
    testUpdater();
}
#endif