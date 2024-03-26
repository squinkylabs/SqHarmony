

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
   
    CompositeUpdater<Comp> c({}, {});
    CVUpdater<Comp> cv(Comp::INPUT1);
    ParamUpdater<Comp> param(Comp::PARAM1);

    // std::vector<CVUpdater<Comp>> v;
    // v.push_back({ Comp::PARAM1 });
    // v.push_back({ Comp::PARAM1 });
    // std::vector<ParamUpdater<Comp>> vpu;
    // vpu.push_back(4);

  //  std::vector<CVUpdater<Comp>> v2(std::move(v));
    // CompositeUpdater<Comp> c2(std::move(vpu), std::move(v));
    // CompositeUpdater<Comp> v3(
    //     {{2}},
    //     {{5}});
    // const bool b = v3.poll();
    SQINFO("Make better testCanCall");
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



static void testPollCVChannels() {
    Comp composite;
    CVUpdater<Comp> cv(Comp::INPUT1);
    CVUpdater<Comp> cv2(Comp::INPUT2);

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
    assertEQ(b, true);      // channels changed
    b = cv2.poll(&composite);
    assertEQ(b, true);      // check underlying value
      b = cv2.poll(&composite);
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