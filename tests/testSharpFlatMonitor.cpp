
#include "Harmony2.h"
#include "KsigSharpFlatMonitor.h"
#include "TestComposite.h"
#include "asserts.h"

class MockPopupMenuParamWidget {
public:
    std::string getShortLabel(unsigned int index) const {
        return {};
    }
    void setLabels(std::vector<std::string> l) {
        SQINFO("mode set labels");
        for (auto x : l) {
            SQINFO(" %s", x.c_str());
        }
    }
};

// const int basePitch = int(std::round(_comp->params[TComp::KEY_PARAM].value));
//       const auto mode = Scale::Scales(int(std::round(_comp->params[TComp::MODE_PARAM].value)));

template <typename T>
class Tester {
public:
    static void testCanCall() {
        T composite;
        MockPopupMenuParamWidget widget;
        KsigSharpFlatMonitor<T, MockPopupMenuParamWidget> mon(&composite, &widget);
        mon.poll();
    }
    static void testFlatDefault() {
        // const int root = MidiNote::F + 1;   // F#
        // const int mode = int(Scale::Scales::Major);
        const int root = MidiNote::D - 1;  // D-
        const int mode = int(Scale::Scales::Major);
        _testX(root, mode, false);

        assert(false);
    }

private:
    static void _testX(int root, int mode, bool expectSharps) {
        // const int root = MidiNote::F + 1;   // F#
        // const int mode = int(Scale::Scales::Major);
        //   const int root = MidiNote::D - 1;   // D-
        //   const int mode = int(Scale::Scales::Major);
        T composite;

        //  using Comp = Harmony2<TestComposite>;
        //  const int x = Comp::KEY_PARAM;
        //   const int keyIndex = T::KEY_PARAM;
        //  const int modeIndex = T::MODE_PARAM;

        composite.params[T::KEY_PARAM].value = root;
        composite.params[T::MODE_PARAM].value = mode;

        MockPopupMenuParamWidget widget;
        KsigSharpFlatMonitor<T, MockPopupMenuParamWidget> mon(&composite, &widget);
        mon.poll();

        assert(false);
    }
};

template <typename T>
void testSharpFlatMonitor() {
    Tester<T>::testCanCall();
}

void testSharpFlatMonitor() {
    // Tester<Harmony2<TestComposite>>::testCanCall();
    testSharpFlatMonitor<Harmony2<TestComposite>>();
}

#if 1
void testFirst() {
    Tester<Harmony2<TestComposite>>::testFlatDefault();
}
#endif