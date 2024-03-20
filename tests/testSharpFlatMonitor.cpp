
#include "Harmony.h"
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
      //  SQINFO("mode set labels");
        lastUpdateSharps = false;
        lastUpdateFlats = false;
        for (auto x : l) {
          //  SQINFO(" %s", x.c_str());
            if (x.find('#') != std::string::npos) {
                lastUpdateSharps = true;
              //  SQINFO("last update sharps");
                return;
            }
            if (x.find('-') != std::string::npos) {
                lastUpdateFlats = true;
               // SQINFO("last update Flats");
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
        // expect flats for d flat
        TestParams test;
        test.scaleRoot = MidiNote::D - 1;  // D-
        test.scaleMode = Scale::Scales::Major;
        test.expectation = Expectations::FlatsSet;
        test.initKeysigToFlatsBeforeTests = false;
        test.initKeysigToSharpsBeforeTests = true;
        test.userPreference = SharpFlatUserOptions::DefaultPlusFlats;
        _testX(test);
        //  _testX(root, mode, 1, true, false, 0);
    }

    static void testDefaultDflat() {
        // test that when already set to a flat key, no change is needed
        // set to flats, , expect no change . In D flat major
        TestParams test;
        test.scaleRoot = MidiNote::D - 1;  // D-
        test.scaleMode = Scale::Scales::Major;
        test.expectation = Expectations::NoneSet;

        test.initKeysigToFlatsBeforeTests = true;
        test.initKeysigToSharpsBeforeTests = false;
        test.userPreference = SharpFlatUserOptions::DefaultPlusSharps;

        // expect flats for d flat
        //    static void _testX(int root, int mode, int expect, bool initToSharps, bool initToFlats, int userPrefs) {
       // _testX(root, mode, 2, false, true, 0);
       _testX(test);
    }

    static void testUserOverrideDflat() {
        // set to flats, normal for  In D flat major.
        // but user pref for sharps

#if 0
        TestParams test;
        test.scaleRoot = MidiNote::D - 1;  // D-
        test.scaleMode = Scale::Scales::Major;
        _testX(root, mode, 0, false, true, 2);
#endif
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
    enum class Expectations {
        SharpsSet,
        FlatsSet,
        NoneSet
    };

    class TestParams {
    public:
        int scaleRoot = 0;
        Scale::Scales scaleMode = Scale::Scales::Major;
        bool initKeysigToSharpsBeforeTests = false;
        bool initKeysigToFlatsBeforeTests = false;

        SharpFlatUserOptions userPreference = SharpFlatUserOptions::DefaultPlusSharps;
        Expectations expectation = Expectations::NoneSet;
    };
    static void _testX(const TestParams& test) {
        //  static void _testX(int root, int mode, int expect, bool initToSharps, bool initToFlats, int userPrefs) {
        assert(!(test.initKeysigToSharpsBeforeTests && test.initKeysigToFlatsBeforeTests));
        T composite;

        composite.params[T::KEY_PARAM].value = test.scaleRoot;
        assert(test.scaleRoot < 12 && test.scaleRoot > 0);
        composite.params[T::MODE_PARAM].value = int(test.scaleMode);
        composite.params[T::SHARPS_FLATS_PARAM].value = int(test.userPreference);

        MockPopupMenuParamWidget widget;
        if (test.initKeysigToSharpsBeforeTests) {
            widget.init(true);
        }
        if (test.initKeysigToFlatsBeforeTests) {
            widget.init(false);
        }
        KsigSharpFlatMonitor<T, MockPopupMenuParamWidget> mon(&composite, &widget);
        mon.poll();

        switch (test.expectation) {
            case Expectations::SharpsSet: {
                assertEQ(widget.lastUpdateSharps, true);
                assertEQ(widget.lastUpdateFlats, false);
            } break;
            case Expectations::FlatsSet: {
                assertEQ(widget.lastUpdateFlats, true);
                assertEQ(widget.lastUpdateSharps, false);
            } break;
            case Expectations::NoneSet: {
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

    w.setLabels({"#"});
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
    testSharpFlatMonitor<Harmony<TestComposite>>();
}

#if 0
void testFirst() {
    testMockWidget();
    Tester<Harmony2<TestComposite>>::testFlatDflat();
}
#endif