
#include "TestLFO.h"
#include "asserts.h"
#include "testShifter6TestUtils.h"

/**
 * These tests are almost all tests the run over a longer period
 * of time, and tend to exercise "real world" scenarios.
 * Every time the show a bug, a more traditional unit tests should be
 * made to test that particular function.
 */
extern bool doLongRunning;


class SignalSourceInterface {
public:
    virtual bool getClock() const = 0;
    virtual float getDelay() const = 0;
    virtual bool isMoreData() const = 0;
    virtual void next() = 0;
};

class SignalSource : public SignalSourceInterface {
 public:
     bool getClock() const override {
         return false;
     }
     float getDelay() const override {
         return 0;
     }
     bool isMoreData() const override {
         return false;
     }
     void next() override {
         assert(false);
     }
private:
};


static void testMakeSource() {
    SignalSource s;
}




void testClockShifter6d() {
    testMakeSource();
    //  SQINFO("start testClockShifter5d");
    assertEQ(SqLog::errorCount, 0);
   // test0();
   // testNoShift();
   // testNoShiftTwice();

    SQINFO("why is test stop not working?");
    // testStop();
  //  testSlowDownOld();

   // testSpeedUp();
   // testSlowDownAndSpeedUp();

   // testSpeedUp(5, 7872, -0.000063516257796437, 10);

   // testWithLFO();
}

#if 1
void testFirst() {
    ClockShifter6::llv = 1;
    //  This is the case that is bad without the dodgy "fix"
   // testWithLFO(4, 16, 0.136364, 0.400000, 3);

   //testSlowDown(5, 3552, 0.0001407658, 7);

}
#endif