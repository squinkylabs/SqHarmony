#pragma once

/**
 * Can represent any pitch, but is in context for a scale
 * 
 */
class ScaleNote {
public:
    /**
     * This used to be called Accidental, but really it's and adjustment from the pitch in the
     * scale.
     * 
     */
    enum class RelativeAdjustment {
        none,
        sharp,
        flat
    };

    ScaleNote(int degree, int oct, RelativeAdjustment a) : _scaleDegree(degree), _octave(oct), _relativeAdjustment(a) {}
    ScaleNote(int degree, int oct) : _scaleDegree(degree), _octave(oct) {}
    ScaleNote() {}
    int getDegree() const { return _scaleDegree; }
    int getOctave() const { return _octave; }
    bool isAccidental() const { return _relativeAdjustment != RelativeAdjustment::none; }
    RelativeAdjustment getAdjustment() const { return _relativeAdjustment; }

    /**
     * @brief 
     * 
     * @param amt - number of steps to transpose.
     * @param stepsInScale - number of steps in the scale. diatonic it's 7. 
     */
    void transposeDegree(int amt, int stepsInScale) {
        _scaleDegree += amt;
        const int lastStepInScale = stepsInScale - 1; 
        while (_scaleDegree > lastStepInScale) {
            _octave++;
            _scaleDegree -= stepsInScale;
        }
        while (_scaleDegree < 0) {
            _octave--;
            _scaleDegree += stepsInScale;
        }
    }

private:
    /**
     * 0 = root, 6 = 7th
     * strictly between 0 and 6 (fully normalized)
     */
    int _scaleDegree = 0;
    int _octave = 0;
    RelativeAdjustment _relativeAdjustment = RelativeAdjustment::none;
};