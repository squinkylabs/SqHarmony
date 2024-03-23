#pragma once

template <typename T>
inline void processBlock(T& composite) {
       const auto args = TestComposite::ProcessArgs();
    // process enough times to do the ribs thing.
    for (int i = 0; i < T::getSubSampleFactor(); ++i) {
        composite.process(args);
    }
}

