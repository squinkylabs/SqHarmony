

#include "Ratchet.h"

#include "asserts.h"

static void testCanCall() {
    Ratchet r;
    (void) r;

    r.setCount(4);
    const bool b =  r.run(true);
    r.trigger();
}

void testRatchet() {
    testCanCall();
  
}