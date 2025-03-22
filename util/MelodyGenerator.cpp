#include "MelodyGenerator.h"

#include "NoteConvert.h"
#include "PitchKnowledge.h"

std::string MelodyRow::print() const {
    std::string ret;
    if (getSize() == 0) {
        return "";
    }
    for (int i = 0; i < getSize(); ++i) {
        const MidiNote& note = notes[i];
        const std::string s = PitchKnowledge::nameOfAbs(note.get());
        ret += s;

        
        const bool isLast = i == getSize() -1;
        if (!isLast) {
            ret += ", ";
        }
    }
    return ret;
}

void MelodyGenerator::mutate(MelodyRow& row, const Scale& scale, MelodyMutateState& state, MelodyMutateStyle& style) {
    assert(style.keepInScale);  // don't know how to do other.
    assert(style.roundRobin);

    changeOneNoteInMode(row, scale, state.nextToMutate ,1);
    state.nextToMutate = MelodyRow::nextNote(state.nextToMutate, row.getSize());
}

void MelodyGenerator::changeOneNoteInMode(MelodyRow& row, const Scale& scale, size_t index, int stepsToChange) {
    assert(index <= row.getSize());

    assert(stepsToChange == 1);  // only think I know how to do.

    ScaleNote scaleNote;
    NoteConvert::m2s(scaleNote, scale, row.getNote(index));
    scaleNote.transposeDegree(stepsToChange, 7);
    MidiNote midiNote;
    NoteConvert::s2m(midiNote, scale, scaleNote);

    row.getNote(index) = midiNote;
}