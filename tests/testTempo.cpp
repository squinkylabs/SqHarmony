
#include "asserts.h"
#include <memory>

// TODO: redo this as a normal 0..1 saw
class Clicker {
public:
    void setPeriodSeconds(float x) {
        assert(x > 0);
        phaseDelta =  1/x;
    }

    // send some time, returns true if clock fires
    bool clockSeconds(float t) {
        bool ret = false;
        acc += t * phaseDelta;
        if (acc > 1) {
            ret = true;
            acc -= 1;
        }
        return ret;
    }

    double phase() const {
        return acc;
    }
private:
    double acc = 0;
  //  double period = .01;
    double phaseDelta = .1;
};

using ClickerPtr = std::shared_ptr<Clicker>;

class ClickerCompare {
public:
    ClickerCompare() {
        a = std::make_shared<Clicker>();
        b = std::make_shared<Clicker>();
    }
    void clockSeconds(float sec) {
        if (a->clockSeconds(sec)) {
            ++aCount;
        }
        if (b->clockSeconds(sec)) {
            ++bCount;
        }
    }

    // return true if A is faster.
    bool whichIsAhead() {
        return getAPhase() > getBPhase();
    }

    // how many periods difference
    double howManyPeriods() {
        return std::abs(getAPhase() - getBPhase());
    }

    double getAPhase() const {
        return aCount + a->phase();
    }

    double getBPhase() const {
        return bCount + b->phase();
    }

    ClickerPtr a;
    ClickerPtr b;
    int aCount = 0;
    int bCount = 0;

};

static void testTempo2() {
    ClickerCompare c;
    c.a->setPeriodSeconds(1);
    c.b->setPeriodSeconds(1);

    // run for 10 seconds
    for (int i=0; i < 100; ++i) {
        c.clockSeconds(.1f);
    }

    // how far ahead is the winnder
    assertEQ(c.howManyPeriods(), 0);
    assertClose(c.getAPhase(), 10, .001);
    assertClose(c.getBPhase(), 10, .001);

}

static void testTempo3() {
    ClickerCompare c;
    c.a->setPeriodSeconds(1);
    c.b->setPeriodSeconds(.9f);

    // clock for one second
    for (int i=0; i < 100; ++i) {
        c.clockSeconds(.01f);
    }

    // b is running faster
    // after 1 second it would be a=1 period, b = 1 /.9 period
    assertEQ(c.whichIsAhead(), false);

    assertClose(c.howManyPeriods(), (1.f / .9f) - 1.f, .001);
}

static void testTempo1() {
    Clicker ck;
    ck.setPeriodSeconds(1);
    bool b = ck.clockSeconds(.25);
    assert(!b);
    b = ck.clockSeconds(.25);
    assert(!b);
    b = ck.clockSeconds(.25);
    assert(!b);
    b = ck.clockSeconds(.26f);
    assert(b);
     b = ck.clockSeconds(.26f);
    assert(!b);
}

static void testTempo1b() {
    Clicker ck;
    ck.setPeriodSeconds(.5);
    bool b = ck.clockSeconds(.25);
    assert(!b);
    b = ck.clockSeconds(.26f);
    assert(b);
}

void testTempo() {

    testTempo1();
    testTempo1b();
    testTempo2();
    testTempo3();

}