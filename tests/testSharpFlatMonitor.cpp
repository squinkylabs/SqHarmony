
#include "Harmony2.h"
#include "KsigSharpFlatMonitor.h"
#include "TestComposite.h"
#include "asserts.h"

class MockPopupMenuParamWidget {
public:
    void setSharps(bool isSharp) {
        if (isSharp) {
            lastUpdateSharps = true;
            useSharps = true;
        } else {
            lastUpdateFlats = true;
            useSharps = false;
        }
    }

    void init(bool isSharps) {
        setSharps(isSharps);
        lastUpdateSharps = false;
        lastUpdateFlats = false;
    }

    std::string getShortLabel(unsigned int index) const {
        assert(index == 1);  // only case we have implemented
        if (useSharps) {
            return {"#"};
        }
    
        return {"-"};
    }

    void setLabels(std::vector<std::string> l) {
        SQINFO("mode set labels");
        lastUpdateSharps = false;
        lastUpdateFlats = false;
        for (auto x : l) {
            SQINFO(" %s", x.c_str());
            if (x.find('#') != std::string::npos) {
                lastUpdateSharps = true;
                SQINFO("last update sharps");
                return;
            }
            if (x.find('-') != std::string::npos) {
                lastUpdateFlats = true;
                SQINFO("last update Flats");
                return;
            }
        }
    }

    bool lastUpdateSharps = false;
    bool lastUpdateFlats = false;
    bool useSharps = true;
};

template <typename T>
class Tester {
public:
    static void testCanCall() {
        T composite;
        MockPopupMenuParamWidget widget;
        KsigSharpFlatMonitor<T, MockPopupMenuParamWidget> mon(&composite, &widget);
        mon.poll();
    }

    static void testFlatDflat() {
        // set to sharps, , expect reset to flats. In D flat major
        const int root = MidiNote::D - 1;  // D-
        const int mode = int(Scale::Scales::Major);
        // expect flats for d flat
        _testX(root, mode, 1, true, false, 0);
    }

    static void testDefaultDflat() {
        // set to flats, , expect no change . In D flat major
        const int root = MidiNote::D - 1;  // D-
        const int mode = int(Scale::Scales::Major);
        // expect flats for d flat
        _testX(root, mode, 2, false, true, 0);
    }

    static void testUserOverrideDflat() {
        // set to flats, normal for  In D flat major.
        // but user pref for sharps
        const int root = MidiNote::D - 1;  // D-
        const int mode = int(Scale::Scales::Major);

        _testX(root, mode, 0, false, true, 2);
     }

    static void testFixture() {
        MockPopupMenuParamWidget widget;
        assertEQ(widget.lastUpdateFlats, false);
        assertEQ(widget.lastUpdateSharps, false);

        widget.setLabels({"xx#yy"});
        assertEQ(widget.lastUpdateFlats, false);
        assertEQ(widget.lastUpdateSharps, true);

        widget.setLabels({"xxyy"});
        assertEQ(widget.lastUpdateFlats, false);
        assertEQ(widget.lastUpdateSharps, false);

        widget.setLabels({"xx-yy"});
        assertEQ(widget.lastUpdateFlats, true);
        assertEQ(widget.lastUpdateSharps, false);
    }

private:
    // expect: 0 = set sharps
    //      1 = set flats
    //      2 = no change

    // user pref:
    /*
      menu->addChild(createMenuItem("Default+sharps", CHECKMARK(index == 0), [=]() {_setSharpFlat(0);}));
                menu->addChild(createMenuItem("Default+flats", CHECKMARK(index == 1), [=]() {_setSharpFlat(1);}));
                menu->addChild(createMenuItem("Sharps", CHECKMARK(index == 2), [=]() {_setSharpFlat(2);}));
                menu->addChild(createMenuItem("Flats", CHECKMARK(index == 3), [=]() {_setSharpFlat(3);}));
    */
    static void _testX(int root, int mode, int expect, bool initToSharps, bool initToFlats, int userPrefs) {
        assert(!(initToSharps && initToFlats));
        T composite;

        composite.params[T::KEY_PARAM].value = root;
        composite.params[T::MODE_PARAM].value = mode;
        composite.params[T::SHARPS_FLATS_PARAM].value = userPrefs;

        MockPopupMenuParamWidget widget;
        if (initToSharps) {
            // widget.setSharps(true);
            widget.init(true);
        }
        if (initToFlats) {
            // widget.setSharps(false);
            widget.init(false);
        }
        KsigSharpFlatMonitor<T, MockPopupMenuParamWidget> mon(&composite, &widget);
        mon.poll();

        switch (expect) {
            case 0: {
                assertEQ(widget.lastUpdateSharps, true);
                assertEQ(widget.lastUpdateFlats, false);
            } break;
            case 1: {
                assertEQ(widget.lastUpdateFlats, true);
                assertEQ(widget.lastUpdateSharps, false);
            } break;
            case 2: {
                assertEQ(widget.lastUpdateSharps, false);
                assertEQ(widget.lastUpdateFlats, false);
            } break;
            default:
                assert(false);
        }
    }
};

template <typename T>
void testSharpFlatMonitor() {
    Tester<T>::testCanCall();
    Tester<T>::testFlatDflat();
    Tester<T>::testDefaultDflat();
    Tester<T>::testUserOverrideDflat();
}

static void testMockWidget() {
    MockPopupMenuParamWidget w;
    assertEQ(w.lastUpdateFlats, false);
    assertEQ(w.lastUpdateSharps, false);

  //  w.init(true);

    w.setLabels({ "#"});
    assertEQ(w.lastUpdateSharps, true);
    assertEQ(w.lastUpdateFlats, false);
    assertEQ(w.getShortLabel(1), "#");

    w.init(false);
    assertEQ(w.lastUpdateSharps, false);
    assertEQ(w.lastUpdateFlats, false);
    assertEQ(w.getShortLabel(1), "-");
}

void testSharpFlatMonitor() {
    // MockPopupMenuParamWidget

    testMockWidget();
    testSharpFlatMonitor<Harmony2<TestComposite>>();
}

#if 0
void testFirst() {
    testMockWidget();
    Tester<Harmony2<TestComposite>>::testFlatDflat();
}
#endif