
#include "KeysigOld.h"
#include "Scale.h"

#if 1

KeysigOld::KeysigOld(Roots rt) {
    scale = std::make_shared<Scale>();

    // Roots start at 1
    const int basePitch = MidiNote::MiddleC + int(rt) - 1;
    const int b = basePitch % 12;
    MidiNote base(b);

    // old one was always major
    scale->set(base, Scale::Scales::Major);
}

void KeysigOld::set(const MidiNote& basePitch, Scale::Scales mode) {
    scale->set(basePitch, mode);
}

ScaleRelativeNote KeysigOld::ScaleDeg(HarmonyNote pitch) {
    const int midiPitch = pitch;
    const MidiNote midiNote(midiPitch);
    const ScaleNote scaleNote = scale->m2s(midiNote);

    ScaleRelativeNote ret;
    if (scaleNote.isAccidental()) {
        // if not in a scale, it's zero
        ret.set(0);
    }
    else {
        int degree = scaleNote.getDegree();
        // if in scale it's 1..7
        ret.set(degree + 1);
    }
    return ret;
}

#else // old impl
Keysig::Keysig(Roots rt) : root(rt) {
    nDegreeTable[0] = 0;  // 0 is not used
    nDegreeTable[1] = 1;
    nDegreeTable[2] = 0;
    nDegreeTable[3] = 2;
    nDegreeTable[4] = 0;
    nDegreeTable[5] = 3;
    nDegreeTable[6] = 4;
    nDegreeTable[7] = 0;
    nDegreeTable[8] = 5;
    nDegreeTable[9] = 0;
    nDegreeTable[10] = 6;
    nDegreeTable[11] = 0;
    nDegreeTable[12] = 7;
}


ScaleRelativeNote Keysig::ScaleDeg(HarmonyNote Pitch) {
    ScaleRelativeNote ret;
    int chro;

    chro = PitchKnowledge::chromaticFromAbs(Pitch);  // get c, c#, etc...
    chro -= (int)root;                 // normalize relative to our root
    chro += 1;                          // root is one, not zero
    if (chro < 1) chro += 12;     // keep positive
    assert(chro > 0 && chro <= 12);
    ret.set(nDegreeTable[chro]);  // coerce us into the ScaleRelativeNote
    return ret;
}
#endif


