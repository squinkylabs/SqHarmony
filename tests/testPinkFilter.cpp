
#include "asserts.h"
#include "PinkFilter.h"


template <typename T>
static void testCanCall() {
    PinkFilter<T> _filter;
    T temp = 0;
    temp = _filter.process(temp);
}
void testPinkFilter() {
   testCanCall<float>();
   testCanCall<float_4>();
}