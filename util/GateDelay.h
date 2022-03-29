
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

class GateDelay {
public:
    GateDelay();
    void process(Input&, unsigned numChannels);
    bool getGate(unsigned channel);
    void enableDelay(bool enabled);

private:
    SchmidtTrigger inputCondition[16];
    bool gates[16]{false};
    static const int size = 5;


    // unsigned delay
    // template <typename T, int SIZE>
    SqRingBuffer<unsigned, size+1> delay;
    bool doDelay = false;
    
    void processDelay(Input&, unsigned numChannels);
    void processNoDelay(Input&, unsigned numChannels);
};

// Turn the ring buffer into a delay line by pushing enough zeros into it
inline GateDelay::GateDelay() : delay(false) {
    // SQINFO("ctor of gate delay here is buffer");
    // ringBuffer._dump();
    for (int i = 0; i < size; ++i) {
        delay.push(0);
        // SQINFO("ctor of gate dela pushed one here is buffer");
        //ringBuffer._dump();
    }
}

inline bool GateDelay::getGate(unsigned channel) {
    assert(channel <= 16);
    return gates[channel];
}

inline void GateDelay::enableDelay(bool enabled) {
    doDelay = enabled;
}

inline void GateDelay::process(Input& input, unsigned numChannels) {
    if (doDelay) {
        processDelay(input, numChannels);
    } else {
        processNoDelay(input, numChannels);
    }
}

inline void GateDelay::processDelay(Input& input, unsigned numChannels) {
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

inline void GateDelay::processNoDelay(Input& input, unsigned numChannels) {
    assert(numChannels <= 16);
    for (unsigned i = 0; i < numChannels; ++i) {
        // auto x = input.getVoltage(i);
        const bool gate = inputCondition[i].go(input.getVoltage(i));
        // SQINFO("input(%d) = %f gate=%d", i, input.getVoltage(i), gate);
        gates[i] = gate;
    }
}