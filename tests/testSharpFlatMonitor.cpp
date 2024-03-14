
#include "Harmony2.h"
#include "KsigSharpFlatMonitor.h"
#include "TestComposite.h"
#include "asserts.h"

class MockPopupMenuParamWidget {
public:
    void setSharps(bool isSharp) {
        if (isSharp) {
            lastUpdateSharps = true;
        } else {
            lastUpdateFlats = true;
        }
    }
    std::string getShortLabel(unsigned int index) const {
        assert(index == 1);  // only case we have implemented
        if (lastUpdateSharps) {
            return {"#"};
        }
        if (lastUpdateFlats) {
            return {"-"};
        }
        return {};
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
        _testX(root, mode, 1, true, false);
    }

    static void testDefaultDflat() {
        // set to flats, , expect no change . In D flat major
        const int root = MidiNote::D - 1;  // D-
        const int mode = int(Scale::Scales::Major);
        // expect flats for d flat
        _testX(root, mode, 2, false, true);
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
    static void _testX(int root, int mode, int expect, bool initToSharps, bool initToFlats) {
        assert(!(initToSharps && initToFlats));
        T composite;

        composite.params[T::KEY_PARAM].value = root;
        composite.params[T::MODE_PARAM].value = mode;

        MockPopupMenuParamWidget widget;
        if (initToSharps) {
            widget.setSharps(true);
        }
        if (initToFlats) {
            widget.setSharps(false);
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
            default:
                assert(false);
        }
    }
};

template <typename T>
void testSharpFlatMonitor() {
    Tester<T>::testCanCall();
}

void testSharpFlatMonitor() {
    testSharpFlatMonitor<Harmony2<TestComposite>>();
}

#if 1
void testFirst() {
    Tester<Harmony2<TestComposite>>::testFixture();
    Tester<Harmony2<TestComposite>>::testFlatDflat();
}
#endif