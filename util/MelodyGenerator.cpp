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

MidiNote MelodyRow::getAveragePitch() const {
    assert(!notes.empty());
    int total = 0;
    int number = 0;
    for (auto note : notes) {
        total += note.get();
        number++;
    }
    return MidiNote(total / number);
}

////////////////////////////

void MelodyGenerator::mutate(MelodyRow& row, const Scale& scale, MelodyMutateState& state, MelodyMutateStyle& style) {
    assert(style.keepInScale);  // don't know how to do other.
    assert(style.roundRobin);

    uint64_t amount64 = state.random();
    const float amountf = 2 * (double(amount64) / double(std::numeric_limits<uint64_t>::max())) - 1;
    int amount = amountf > 0 ? 1 : -1;
    _changeOneNoteInMode(row, scale, state.nextToMutate, amount);
    state.nextToMutate = MelodyRow::nextNote(state.nextToMutate, row.getSize());
}

void MelodyGenerator::_changeOneNoteInMode(MelodyRow& row, const Scale& scale, size_t index, int stepsToChange) {
    assert(index <= row.getSize());

   // assert(stepsToChange == 1);  // only thing I know how to do.

    ScaleNote scaleNote;
    NoteConvert::m2s(scaleNote, scale, row.getNote(index));
    scaleNote.transposeDegree(stepsToChange, 7);
    MidiNote midiNote;
    NoteConvert::s2m(midiNote, scale, scaleNote);

    row.getNote(index) = midiNote;
}