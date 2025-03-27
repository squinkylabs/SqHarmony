#include "MelodyGenerator.h"

#include "NoteConvert.h"
#include "PitchKnowledge.h"

#include <algorithm>

std::string MelodyRow::print() const {
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

float MelodyEvaluator::getPenalty(const MelodyRow& r) {
    float totalPenalty = 0;
    totalPenalty += leapsPenalty(r);

    SQINFO("returning penalty %f for row %s", totalPenalty, r.print().c_str());

    return totalPenalty;
}

float MelodyEvaluator::leapsPenalty(const MelodyRow& r) {
    int bigLeaps = 0;
    for (int i=0; i < r.getSize(); ++i) {
        const MidiNote& note1 = r.getNote(i);
        const MidiNote& note2 = r.getNote(i + 1);
        const int jump = std::abs( note1.get() - note2.get());
      
        if (jump > 4) {
            bigLeaps++;
        }
        //SQINFO("i=%d jump=%d big leaps=%d", i, jump, bigLeaps);
    }
    return float(bigLeaps) / float(r.getSize());
}

////////////////////////////

int pickOne(int numBest, int bestCandidates[], MelodyMutateState& state) {
    assert(numBest > 0);

    const float rand = double(state.random()) * double(numBest-1) / (std::numeric_limits<uint64_t>::max());
    const int randIndex = std::round(rand);
    assert(randIndex < numBest);
    assert(randIndex >= 0);
    const int selectedCandidate = bestCandidates[randIndex];

    SQINFO("pickOne(%d) rand=%f randIndex=%d", numBest, rand, randIndex);
    for (int i=0; i<numBest; ++i) {
        SQINFO("  bestCandidates[%d] = %d", i, bestCandidates[i]);
    }
    SQINFO("pickOne returned %d", selectedCandidate);
    return selectedCandidate;
}

void MelodyGenerator::mutate(MelodyRow& row, const Scale& scale, MelodyMutateState& state, const MelodyMutateStyle& style) {
    assert(style.keepInScale);  // don't know how to do other.
    assert(style.roundRobin);

    int candidateShifts[] = {-2, -1, 1, 2, 0};
    MelodyRow mutatedCandidates[4];
    float penalties[4];
    float lowestPenalty = 1000;       // insanely high penalty

    // First, make all the mutation candidates
    for (int i=0; 0 != candidateShifts[i]; ++i) {
        mutatedCandidates[i] = row;
        _changeOneNoteInMode(mutatedCandidates[i], scale, state.nextToMutate, candidateShifts[i]);
        const float penalty = MelodyEvaluator::getPenalty(mutatedCandidates[i]); 
        penalties[i] = penalty;
        lowestPenalty = std::min(penalty, lowestPenalty);

        SQINFO("i=%d, penalty=%f lowest=%f", i, penalty, lowestPenalty);
    }

    // Next find which candidates are best.
    int bestCandidates[5];
    int index = 0;
    for (int i=0; i < 4; ++i) {
        const bool isLowest = (penalties[i] == lowestPenalty);
        if (isLowest) {
            bestCandidates[index++] = i;  
        }
    }
   
    SQINFO("found num=%d  0=%d 1=%1 2=%d 3=%d", index, bestCandidates[0], bestCandidates[1], bestCandidates[2], bestCandidates[3]);

    // Randomly pick one of the best
    const int theRow = pickOne(index, bestCandidates, state);
    row = mutatedCandidates[theRow];

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

    //  row.getNote(index) = midiNote;
    row.setNote(index, midiNote);
}