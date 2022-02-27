
#include "NoteConvert.h"

#include <assert.h>
#include <cmath>

#include "FloatNote.h"
#include "MidiNote.h"
#include "Scale.h"
#include "ScaleNote.h"

void NoteConvert::m2f(FloatNote& fnote, const MidiNote& midiNote) {
    int x = midiNote.get() - (MidiNote::C3 + 12);  // midi 60 is C3 is -1 volts in VCV
    float f = float(x) / 12.f;
    fnote = FloatNote(f);
};

void NoteConvert::f2m(MidiNote& midiNote, const FloatNote& fNote) {
    float x = fNote.get();
    x *= 12.f;
    x += (MidiNote::C3 + 12);       // 0v is c4

    int i = int(std::round(x));
    midiNote = MidiNote(i);
}

void NoteConvert::s2m(MidiNote& midiNote, const Scale& scale, const ScaleNote& scaleNote) {
    midiNote = scale.s2m(scaleNote);
}

void NoteConvert::m2s(ScaleNote& sn, const Scale& scale, const MidiNote& mn) {
    sn = scale.m2s(mn);
}

void NoteConvert::f2s(ScaleNote& sn, const Scale& scale, const FloatNote& fn) {
    MidiNote mn;
    f2m(mn, fn);
    sn = scale.m2s(mn);
}

void NoteConvert::s2f(FloatNote& fn, const Scale& scale, const ScaleNote& sn) {
    MidiNote mn = scale.s2m(sn);
    m2f(fn, mn);
}