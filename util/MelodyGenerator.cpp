#include "MelodyGenerator.h"

#include "FloatNote.h"
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

float MelodyEvaluator::getPenalty(const MelodyRow& r, const MelodyMutateStyle& style) {
    float totalPenalty = 0;
    totalPenalty += leapsPenalty(r);
    totalPenalty += unisonsPenalty(r);
    totalPenalty += nonCenteredPenalty(r, style);


    //SQINFO("returning penalty %f for row %s", totalPenalty, r.print().c_str());

    return totalPenalty;
}

float MelodyEvaluator::leapsPenalty(const MelodyRow& r) {
    int bigLeaps = 0;
    for (size_t i=0; i < r.getSize(); ++i) {
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

float MelodyEvaluator::unisonsPenalty(const MelodyRow& r) {
    int unisons = 0;
    for (size_t i=0; i < r.getSize(); ++i) {
        const MidiNote& note1 = r.getNote(i);
        const MidiNote& note2 = r.getNote(i + 1);
       if (note1.get() == note2.get()) {
        unisons++;
       } 
    }

    // a single unison doesn't count.
    if (unisons <= 1) {
        return 0;
    }
    return float(unisons) / float(r.getSize());
}

float MelodyEvaluator::nonCenteredPenalty(const MelodyRow& r, const MelodyMutateStyle& style) {
   // SQINFO("enter eval non cent, row=%s", r.print().c_str());
    assert(r.getSize() > 0);
    float totalDeviation = 0;
    FloatNote floatTarget(style.centerVoltage);
    for (size_t i=0; i < r.getSize(); ++i) {
        const MidiNote& note = r.getNote(i);
        FloatNote floatNote;
        NoteConvert::m2f(floatNote, note);
      

       // totalDeviation += std::abs(note.get() - MidiNote::MiddleC);
        totalDeviation += std::abs(floatNote.get() - floatTarget.get());
       // SQINFO("in loop, note=%d total dev = %f", note.get(), totalDeviation);
    }

    const float penalty = totalDeviation / r.getSize();

  //  SQINFO("nonCenetered, PENALTY=%f total dev = %d", penalty, totalDeviation);

    // maj and minor centered with 1
    // and with .1
    // and with .01
    // and .0001
    // and .0000001
    // and .000000001
    // ng with  .00000000001
    // .0000000001 is good for now
    //assert(false);      // re-do this
    return penalty * .0000000001;
};

////////////////////////////

int pickOne(int numBest, int bestCandidates[], MelodyMutateState& state) {
    assert(numBest > 0);

    const float rand = double(state.random()) * double(numBest-1) / (std::numeric_limits<uint64_t>::max());
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
        const float penalty = MelodyEvaluator::getPenalty(mutatedCandidates[i], style); 
        penalties[i] = penalty;
        lowestPenalty = std::min(penalty, lowestPenalty);

      ///  SQINFO("i=%d, penalty=%f lowest=%f", i, penalty, lowestPenalty);
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
   
  //  SQINFO("found num=%d  0=%d 1=%d 2=%d 3=%d", index, bestCandidates[0], bestCandidates[1], bestCandidates[2], bestCandidates[3]);

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