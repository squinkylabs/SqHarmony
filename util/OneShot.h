#pragma once

#include <assert.h>

/**
 * @brief generates a single, fixed length pulse.
 * 
 * Note that the class is meant to generate fixed length, in "clock time"
 * (seconds) rather than samples. Good for generating a VCV standard trigger.
 * 
 */
class OneShot
{
public:
    /**
     * @brief note one more sample time elapsed
     */
    void step();
    /**
     * @brief note more time has elapsed. Note that this
     * overloaded step does not require that setSampleTime be called;
     * 
     * @param seconds is the amount of elapsed time
     */
    void step(float seconds);
    bool hasFired() const;   
    void setDelayMs(float milliseconds);

    void setSampleTime(float);

    /**
     * start the one shot.
     *      It will go into the !fired state.
     *      will start counting.
     */
    void set();
private:
    float sampleTime = 0;
    float delayMs = 0;
    float accumulatorSec = 0;
    float deadlineSec = 0;
    bool fired = true;
    void update();
};

inline void OneShot::step(float seconds)
{
    if (fired) {
        return;
    }
    assert(seconds > 0);
    assert(delayMs > 0);
    accumulatorSec += seconds;
    if (accumulatorSec >= deadlineSec) {
        fired = true;
    }
}

inline void OneShot::step()
{
    step(sampleTime);
}

inline void OneShot::update()
{
    deadlineSec = delayMs / 1000;
    accumulatorSec = 0;
}

inline void OneShot::setSampleTime(float seconds)
{
    sampleTime = seconds;
}

inline void OneShot::setDelayMs(float millis)
{
    delayMs = millis;
    update();
}

inline bool OneShot::hasFired() const
{
    return fired;
}

inline void OneShot::set()
{
    fired = false;
    accumulatorSec = 0;
}

