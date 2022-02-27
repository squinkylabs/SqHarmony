#pragma once

class FloatNote {
public:
    FloatNote() : pitch(0) {}
    FloatNote(float p) : pitch(p) {}
    float get() const { return pitch; }
private:
    float pitch = 0;
};