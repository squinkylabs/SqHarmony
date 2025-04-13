#include "MelodyGenerator.h"

#include <algorithm>

#include "MelodyEvaluator.h"
#include "NoteConvert.h"
#include "PitchKnowledge.h"

std::string MelodyRow::toString() const {
    std::string ret;
    if (getSize() == 0) {
        return "";
    }
    for (size_t i = 0; i < getSize(); ++i) {
        const MidiNote& note = this->getNote(i);
        const std::string s = PitchKnowledge::nameOfAbs(note.get());
        ret += s;

        const bool isLast = i == getSize() - 1;
        if (!isLast) {
            ret += ", ";
        }
    }
    return ret;
}

MidiNote MelodyRow::getAveragePitch() const {
    assert(!empty());
    int total = 0;
    int number = 0;
    for (size_t i = 0; i < getSize(); ++i) {
        const MidiNote& note = this->getNote(i);
        total += note.get();
        number++;
    }
    return MidiNote(total / number);
}
///////////////////////////////

////////////////////////////

int pickOne(int numBest, int bestCandidates[], MelodyMutateState& state) {
    assert(numBest > 0);

    const float rand = double(state.random()) * double(numBest - 1) / (std::numeric_limits<uint64_t>::max());
    const int randIndex = std::round(rand);
    assert(randIndex < numBest);
    assert(randIndex >= 0);
    const int selectedCandidate = bestCandidates[randIndex];

    // SQINFO("pickOne(%d) rand=%f randIndex=%d", numBest, rand, randIndex);
    // for (int i=0; i<numBest; ++i) {
    //     SQINFO("  bestCandidates[%d] = %d", i, bestCandidates[i]);
    // }
    // SQINFO("pickOne returned %d", selectedCandidate);
    return selectedCandidate;
}

void MelodyGenerator::mutate(MelodyRow& row, const Scale& scale, MelodyMutateState& state, const MelodyMutateStyle& style) {
    if (style.numToMutate == 1) {
        const size_t noteIndex = state.nextToMutate;
        assert(style.numToMutate == 1);
        _mutateOne(row, noteIndex, scale, state, style);
       // state.nextToMutate = MelodyRow::nextNote(state.nextToMutate, row.getSize());
        state.nextToMutate = row.wrapIndex(state.nextToMutate + 1);
        return;
    }
    assert(style.roundRobin == true);
    assert(style.mutateAdjacent == true);
    assert(row.getSize() <= 16);

    int toMutate[16+1];
    int index = 0;
    const int numThisTime = std::min(row.getSize(), size_t(style.numToMutate));
    for (int i = 0; i < numThisTime; ++i) {
        int x = state.nextToMutate + i;
        // if (x >= row.getSize()) {           // make this a method on row?
        //     x -= row.getSize();
        // }
        x = row.wrapIndex(x);
        assert(x < row.getSize());
        toMutate[index++] = x;
    }
    toMutate[index] = -1;
    _mutateSome(row, scale, state, style, toMutate);

}

void MelodyGenerator::_mutateSome(MelodyRow& row, const Scale& scale, MelodyMutateState& state, const MelodyMutateStyle& style, int* indiciesToMutate) {
    for (int i=0; indiciesToMutate[i] >= 0; ++i) {
        _mutateOne(row, i, scale, state, style);
    }    
}

void MelodyGenerator::_mutateOne(MelodyRow& row, size_t noteIndex, const Scale& scale, MelodyMutateState& state, const MelodyMutateStyle& style) {
    assert(style.keepInScale);  // don't know how to do other.
    assert(style.roundRobin);

    int candidateShifts[] = {-2, -1, 1, 2, 0};
    MelodyRow mutatedCandidates[4];
    float penalties[4];
    float lowestPenalty = 1000;  // insanely high penalty

    // First, make all the mutation candidates
    for (int i = 0; 0 != candidateShifts[i]; ++i) {
        mutatedCandidates[i] = row;
        _changeOneNoteInMode(mutatedCandidates[i], scale, noteIndex, candidateShifts[i]);
        const float penalty = MelodyEvaluator::getPenalty(mutatedCandidates[i], style);
        penalties[i] = penalty;
        lowestPenalty = std::min(penalty, lowestPenalty);

        ///  SQINFO("i=%d, penalty=%f lowest=%f", i, penalty, lowestPenalty);
    }

    // Next find which candidates are best.
    int bestCandidates[5];
    int index = 0;
    for (int i = 0; i < 4; ++i) {
        const bool isLowest = (penalties[i] == lowestPenalty);
        if (isLowest) {
            bestCandidates[index++] = i;
        }
    }

    //  SQINFO("found num=%d  0=%d 1=%d 2=%d 3=%d", index, bestCandidates[0], bestCandidates[1], bestCandidates[2], bestCandidates[3]);

    // Randomly pick one of the best
    const int theRow = pickOne(index, bestCandidates, state);
    row = mutatedCandidates[theRow];

    // SQINFO("%s", MelodyEvaluator::toString(row, style).c_str());
}

void MelodyGenerator::_changeOneNoteInMode(MelodyRow& row, const Scale& scale, size_t index, int stepsToChange) {
    assert(index <= row.getSize());

    // assert(stepsToChange == 1);  // only thing I know how to do.

    ScaleNote scaleNote;
    NoteConvert::m2s(scaleNote, scale, row.getNote(index));
    scaleNote.transposeDegree(stepsToChange, 7);
    MidiNote midiNote;
    NoteConvert::s2m(midiNote, scale, scaleNote);

    //  row.getNote(index) = midiNote;
    row.setNote(index, midiNote);
}