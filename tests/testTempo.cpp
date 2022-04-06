
#include "asserts.h"

class Clicker {
public:
    void setPeriodSeconds(float x) {
        assert(x > 0);
        period = x;
    }

    // send some time, returns true if clock fires
    bool clockSeconds(float t) {
        bool ret = false;
        acc += t;
        if (acc > period) {
            ret = true;
            acc -= period;
        }
        return ret;
    }
private:
    double acc = 0;
    double period = .01;
};

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

void testTempo() {
    testTempo1();
}