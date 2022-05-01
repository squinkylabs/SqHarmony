
#pragma once

#ifdef __PLUGIN
    #include "rack.hpp"
    using Input = struct rack::engine::Input;
#else
    #include "TestComposite.h"
#endif

#include <assert.h>

#include "SchmidtTrigger.h"
#include "SqLog.h"
#include "SqRingBuffer.h"

template <int SIZE=5>
class GateDelay {
public:
    GateDelay();
    void process(Input&, unsigned numChannels);
    bool getGate(unsigned channel);
    void enableDelay(bool enabled);

private:
    SchmidtTrigger inputCondition[16];
    bool gates[16]{false};

    SqRingBuffer<unsigned, SIZE+1> delay;
    bool doDelay = false;
    
    void processDelay(Input&, unsigned numChannels);
    void processNoDelay(Input&, unsigned numChannels);
};

// Turn the ring buffer into a delay line by pushing enough zeros into it
template <int SIZE>
inline GateDelay<SIZE>::GateDelay() : delay(false) {
    for (int i = 0; i < SIZE; ++i) {
        delay.push(0);
    }
}

template <int SIZE>
inline bool GateDelay<SIZE>::getGate(unsigned channel) {
    assert(channel <= 16);
    return gates[channel];
}

template <int SIZE>
inline void GateDelay<SIZE>::enableDelay(bool enabled) {
    doDelay = enabled;
}

template <int SIZE>
inline void GateDelay<SIZE>::process(Input& input, unsigned numChannels) {
    if (doDelay) {
        processDelay(input, numChannels);
    } else {
        processNoDelay(input, numChannels);
    }
}

template <int SIZE>
inline void GateDelay<SIZE>::processDelay(Input& input, unsigned numChannels) {
    assert(numChannels <= 16);
    unsigned x = 0;
    for (unsigned i = 0; i < numChannels; ++i) {
        const bool gate = inputCondition[i].go(input.getVoltage(i));
        if (gate) {
            x |= (1 << i);
        }
    }

    if (!delay.empty()) {
        auto y = delay.pop();
        for (unsigned i = 0; i < numChannels; ++i) {
            gates[i] = y & (1 << i);
        }
    }
    delay.push(x);
}

template <int SIZE>
inline void GateDelay<SIZE>::processNoDelay(Input& input, unsigned numChannels) {
    assert(numChannels <= 16);
    for (unsigned i = 0; i < numChannels; ++i) {
        // auto x = input.getVoltage(i);
        const bool gate = inputCondition[i].go(input.getVoltage(i));
        // SQINFO("input(%d) = %f gate=%d", i, input.getVoltage(i), gate);
        gates[i] = gate;
    }
}