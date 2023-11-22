
#pragma once
class Keysig;

class ScaleRelativeNote {
public:
    bool isValid() const;
    operator int() const { return pitch; }         // convert us to int
    int interval(const ScaleRelativeNote&) const;  // tell interval between these guys
    bool isTonal() const;                          // am I "tonal"?

    void setScaleDegree(int);  // I should be private!!!!!
    bool isLeadingTone() const;
    // TODO: actually const, ctor should be private?

private:
    int pitch = 0;  // valid values are 1..8!
    friend Keysig;  // so he can call set on us
};

inline int ScaleRelativeNote::interval(const ScaleRelativeNote& Higher) const {
    int ret;
    ret = Higher + 1 - pitch;
    if (ret <= 0) ret += 7;
    return ret;
}

inline void ScaleRelativeNote::setScaleDegree(int nPitch) {
    pitch = nPitch;
}

inline bool ScaleRelativeNote::isValid() const {
    return (pitch >= 1) && (pitch <= 7);
}
