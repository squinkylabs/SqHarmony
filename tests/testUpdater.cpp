

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

    std::vector<CVUpdater<Comp>> v;
    v.push_back({ &composite,  int(Comp::PARAM1) });
    v.push_back({ &composite,  Comp::PARAM1 });

    std::vector<ParamUpdater<Comp>> vpu;
    vpu.push_back({&composite, 4});

   std::vector<CVUpdater<Comp>> v2(std::move(v));

    CompositeUpdater<Comp> c2(std::move(vpu), std::move(v));

    CompositeUpdater<Comp> v3(
        { {&composite, 2} },
        { {&composite, 5} });

}

class foo {
public:
    std::vector<int> v;
  foo(std::vector<int>&& x) : v(std::move(x)) {}
};




static void testExp() {

    foo f = std::vector<int>{ 1,2,3 };

    std::vector<int> g = std::vector<int>{ 1,2,3 };
    SQINFO("f=%d g=%d", f.v.size(), g.size());
    foo h(std::move(g));
    SQINFO("h=%d g=%d", h.v.size(), g.size());
}

void testUpdater() {
    testCanCall();
    testExp();
}

#if 1
void testFirst() {
    // i == 0, j== -2
    // testModeCV(-14, 0, true);
    testUpdater();
}
#endif