
#include "Harmony2.h"
#include "TestComposite.h"
#include "KsigSharpFlatMonitor.h"
#include "asserts.h"

class MockPopupMenuParamWidget {
public:
    std::string getShortLabel(unsigned int index) const {
        return {};
    }
    void setLabels(std::vector<std::string> l) {

    }
};

template <typename T>
class Tester {
public:
    static void testCanCall() {
      //  KsigSharpFlatMonitor(const TComp * comp, PopupMenuParamWidget * rootWidget)
        T composite;
        MockPopupMenuParamWidget widget;

        KsigSharpFlatMonitor<T, MockPopupMenuParamWidget> mon(&composite, &widget);
        mon.poll();
    }
};



void testSharpFlatMonitor() {
    Tester<Harmony2<TestComposite>>::testCanCall();
}


#if 0
void testFirst() {
   Tester<Harmony2<TestComposite>>::testCanCall();
}
#endif