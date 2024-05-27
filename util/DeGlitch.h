#pragma once

#include <assert.h>

class DeGlitch {
public:
    /**
     * @brief 
     * 
     * @param input is a clock input
     * @param lockoutCount is how long to lock out after low to high. if zero, no lock out.
     * @return true 
     * @return false 
     */
    bool process(bool input, unsigned lockoutCount);
private:
    unsigned _count = 0;
    unsigned _lastOutput = 0;
};

inline bool DeGlitch::process(bool input, unsigned lockoutCount) {
    ++_count;
    if (lockoutCount == 0) {
        return input;
    }
    if (_lastOutput == 0) {
        return input;
    }
    assert(false);
    
    return false;
}