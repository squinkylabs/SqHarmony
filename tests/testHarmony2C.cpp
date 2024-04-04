
#include "Scale.h"
#include "Harmony2.h"
#include "TestComposite.h"

#include "asserts.h"


using Comp = Harmony2<TestComposite>;
using CompPtr = std::shared_ptr<Comp>;


static void hookUpCV(Comp&) {

}
static CompPtr makeComp(Scale& fromScale) {
    CompPtr composite = std::make_shared<Comp>();
    hookUpCV(*composite);
    return composite;
}


static void testPESOutput() {
    auto comp = makeComp;
    assert(false);
}

void testHarmony2C() {
   testPESOutput();
}


#if 1
void testFirst() {
    SQINFO("Test First");
    testPESOutput();
}
#endif