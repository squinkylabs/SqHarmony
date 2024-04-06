

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
    enum OutputIds {
        OUTPUT1,
        OUTPUT2
    };
};

using Comp = MyComposite<TestComposite>;

static void testCanCall() {
    Comp composite;
    CompositeUpdater<Comp> c(&composite);
    CVInUpdater<Comp> cv(Comp::INPUT1, true);
    ParamUpdater<Comp> param(Comp::PARAM1);

    c.add(Comp::PARAM1);
    c.add(Comp::INPUT1, false);
}

static void testPollParam() {
    Comp composite;
    ParamUpdater<Comp> pu(Comp::PARAM1);
    ParamUpdater<Comp> pu2(Comp::PARAM2);

    bool b = pu.poll(&composite);
    assertEQ(b, true);
    b = pu.poll(&composite);
    assertEQ(b, false);

    b = pu2.poll(&composite);
    assertEQ(b, true);

    composite.params[Comp::PARAM2].value = 66;

    b = pu.poll(&composite);
    assertEQ(b, false);
    b = pu2.poll(&composite);
    assertEQ(b, true);

    b = pu.poll(&composite);
    assertEQ(b, false);
    b = pu2.poll(&composite);
    assertEQ(b, false);
}

static void testPollCVInChannels() {
    Comp composite;
    CVInUpdater<Comp> cv(Comp::INPUT1, true);
    CVInUpdater<Comp> cv2(Comp::INPUT2, false);

    bool b = cv.poll(&composite);
    assertEQ(b, true);
    b = cv2.poll(&composite);
    assertEQ(b, true);

    b = cv.poll(&composite);
    assertEQ(b, false);
    b = cv2.poll(&composite);
    assertEQ(b, false);

    auto& in1 = composite.inputs[Comp::INPUT1];
    auto& in2 = composite.inputs[Comp::INPUT2];

    in2.channels = 5;
    b = cv.poll(&composite);
    assertEQ(b, false);
    b = cv2.poll(&composite);
    assertEQ(b, true);  // channels changed
    b = cv2.poll(&composite);
    assertEQ(b, false);
    b = cv2.poll(&composite);
    assertEQ(b, false);
}

static void testPollCVOutChannels() {
    Comp composite;
    CVOutUpdater<Comp> cv(Comp::OUTPUT1);
    CVOutUpdater<Comp> cv2(Comp::OUTPUT2);

    bool b = cv.poll(&composite);
    assertEQ(b, true);
    b = cv2.poll(&composite);
    assertEQ(b, true);

    b = cv.poll(&composite);
    assertEQ(b, false);
    b = cv2.poll(&composite);
    assertEQ(b, false);

    auto& out1 = composite.outputs[Comp::OUTPUT1];
    auto& out2 = composite.outputs[Comp::OUTPUT2];

    out2.channels = 5;
    b = cv.poll(&composite);
    assertEQ(b, false);
    b = cv2.poll(&composite);
    assertEQ(b, true);  // channels changed
    b = cv2.poll(&composite);
    assertEQ(b, false);
    b = cv2.poll(&composite);
    assertEQ(b, false);
}

static void testPollCVInValues() {
    Comp composite;
    CVInUpdater<Comp> cv(Comp::INPUT1, false);

    auto& in1 = composite.inputs[Comp::INPUT1];
    in1.channels = 3;      // make it polyphonic

    // initial start up poll
    bool b = cv.poll(&composite);
    assertEQ(b, true);

    b = cv.poll(&composite);
    assertEQ(b, false);
    in1.setVoltage(3);
    b = cv.poll(&composite);
    assertEQ(b, true);

    b = cv.poll(&composite);
    assertEQ(b, false);

    SQINFO("!! MORE TESTS FOR MONO/POLY");

}

static void testPollCVOutValues() {
    Comp composite;
    CVOutUpdater<Comp> cv(Comp::OUTPUT1);

    auto& out1 = composite.outputs[Comp::OUTPUT1];
    out1.channels = 3;      // make it polyphonic

    // initial start up poll
    bool b = cv.poll(&composite);
    assertEQ(b, true);

    b = cv.poll(&composite);
    assertEQ(b, false);
    out1.setVoltage(3);
    b = cv.poll(&composite);            // output poll doesn't look at values
    assertEQ(b, false);

    b = cv.poll(&composite);
    assertEQ(b, false);


}
static void testPollCompositeUpdater_oneParam() {
    Comp composite;
    CompositeUpdater<Comp> up;
    up.set(&composite,100);
    up.add(Comp::PARAM1);

    bool b  = up.poll();
    assertEQ(b, true);
    b = up.poll();
    assertEQ(b, false);
    b = up.poll();
    assertEQ(b, false);
}

static void testPollCompositeUpdater_oneCVMono() {
    //  assert(false);
    SQINFO("----t testPollCompositeUpdater");
    Comp composite;
    CompositeUpdater<Comp> up;
    up.set(&composite, 100);
    composite.inputs[Comp::INPUT1].channels = 1;
    up.add(Comp::INPUT1, true);

    bool b = up.poll();
    assertEQ(b, true);
    b = up.poll();
    assertEQ(b, false);
    b = up.poll();
    assertEQ(b, false);

    composite.inputs[Comp::INPUT1].setVoltage(11, 0);
    b = up.poll();
    assertEQ(b, true);
    b = up.poll();
    assertEQ(b, false);
}

static void testPollCompositeUpdater_InfrequentParam1() {
    Comp composite;
    CompositeUpdater<Comp> up;
    up.set(&composite,4);
   
   up.addInfrequentParam(Comp::PARAM1);


    bool b  = up.poll();
    assertEQ(b, true);
    b = up.poll();
    assertEQ(b, false);
    b = up.poll();
    assertEQ(b, false);
}

static void testPollCompositeUpdater_InfrequentParam2() {
    Comp composite;
    CompositeUpdater<Comp> up;
    up.set(&composite,4);
   
    up.addInfrequentParam(Comp::PARAM1);
    auto &param = composite.params[Comp::PARAM1];


    // always see a change the first time
    bool b  = up.poll();
    assertEQ(b, true);

    // even with a change don't see change frequently
    param.value = 123;
    b = up.poll();
    assertEQ(b, false);
    b = up.poll();
    assertEQ(b, false);
    b = up.poll();
    assertEQ(b, false);

     b = up.poll();
    assertEQ(b, true);

}

void testUpdater() {
    testCanCall();
    testPollParam();
    testPollCVInChannels();
    testPollCVInValues();
    testPollCVOutChannels();
    testPollCVOutValues();
    testPollCompositeUpdater_oneParam();
    testPollCompositeUpdater_oneCVMono();
    testPollCompositeUpdater_InfrequentParam1();
    testPollCompositeUpdater_InfrequentParam2();
}

#if 0
void testFirst() {
    testPollCompositeUpdater_InfrequentParam2();
}
#endif