#pragma once

class FreqMeasure {
public:
    void onSample(bool trigger);
    bool freqValid() const;
    int64_t getPeriod() const;


};

inline void  FreqMeasure::onSample(bool trigger) {
}

inline bool  FreqMeasure::freqValid() const {
    return false;
}
