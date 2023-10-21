#pragma once

class ClockWithPhase {
public:
    ClockWithPhase() {}
    ClockWithPhase(int clocks, float phase) : _clocks(clocks), _phase(phase) {}

    int _clocks = 0;
    float _phase = 0;
};

class ClockWithSamples {
public:
    ClockWithSamples() {}
    ClockWithSamples(int clocks, int samples) : _clocks(clocks), _samples(samples) {}

    int _clocks = 0;
    int _samples = 0;
};

// class ShiftMath {
//     static void clock

// };
