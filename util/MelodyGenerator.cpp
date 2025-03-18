#include "MelodyGenerator.h"

#include "NoteConvert.h"

void MelodyGenerator::mutate(MelodyRow& row, const Scale& scale, MelodyMutateState& state, MelodyMutateStyle& style) {
    assert(style.keepInScale);  // don't know how to do other.

    changeOneNoteInMode(row, scale, 0, 1);
}

void MelodyGenerator::changeOneNoteInMode(MelodyRow& row, const Scale& scale, size_t index, int stepsToChange) {
    assert(index <= row._getSize());

    assert(stepsToChange == 1);  // only think I know how to do.

    ScaleNote scaleNote;
    NoteConvert::m2s(scaleNote, scale, row.getNote(index));
    scaleNote.transposeDegree(stepsToChange, 7);
    MidiNote midiNote;
    NoteConvert::s2m(midiNote, scale, scaleNote);

    row.getNote(index) = midiNote;
}