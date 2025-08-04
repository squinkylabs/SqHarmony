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

   // const float rand = double(state.random()) * double(numBest - 1) / (std::numeric_limits<uint64_t>::max());
    assert(false);
    const int randIndex = 0;
    // const int randIndex = std::round(rand);


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

void MelodyGenerator::makeStateLegal(MelodyMutateState& state, const MelodyRow& row) {
    if (state.nextToMutate >= row.getSize()) {
        state.nextToMutate = 0;
    }
}

void MelodyGenerator::mutate(MelodyRow& row, const Scale& scale, MelodyMutateState& state, const MelodyMutateStyle& style) {
    //   assert(style.numToMutate == 1);

    makeStateLegal(state, row);

    // assert(style.roundRobin == true);
    assert(style.adjacentStyle == 0);
    assert(row.getSize() <= 16);

    // special case for only one. It's a common case, and faster/
    if (style.numToMutate == 1) {
        const size_t noteIndex = state.nextToMutate;
        assert(style.numToMutate == 1);
        _mutateOne(row, noteIndex, scale, state, style);
        state.nextToMutate = row.wrapIndex(state.nextToMutate + 1);
        // SQINFO("in mutate: advance next 65 to %d", state.nextToMutate);
        return;
    }

    int toMutate[16 + 1];
    getIndiciesToMutate(row, scale, state, style, toMutate);
    SQINFO("to mutate = %d %d %d %d %d", toMutate[0], toMutate[1], toMutate[2], toMutate[3], toMutate[4]);
    _mutateSome(row, scale, state, style, toMutate);
}

bool MelodyGenerator::toMutateIncludes(const int* indiciesToMutate, int candidateIndex) {
    assert(candidateIndex >= 0);
    for(const int* p = indiciesToMutate; ; ++p) {
        const int x = *p;
        if (x < 0) {
            return false;
        }
        if (x == candidateIndex) {
            return true;
        }
    }
}

void MelodyGenerator::getIndiciesToMutate(MelodyRow& row, const Scale& scale, MelodyMutateState& state, const MelodyMutateStyle& style, int* indiciesToMutate) {
    const int numThisTime = std::min(row.getSize(), size_t(style.numToMutate));
    int index = 0;
    indiciesToMutate[0] = -1;  // init to zero length.
    if (style.numToMutate == 0) {
        size_t i;
        for (i = 0; i < row.getSize(); ++i) {
            indiciesToMutate[i] = i;
        }
        index = i;  // so that at the end we can terminate

    } else if (style.adjacentStyle == 1) {
        assert(false);
        for (int i = 0; i < numThisTime; ++i) {
            int x = state.nextToMutate + i;
            // SQINFO("use at 90  %d", state.nextToMutate);
            x = row.wrapIndex(x);
            assert(x < (int)row.getSize());
            indiciesToMutate[index++] = x;
        }
        state.nextToMutate = row.wrapIndex(state.nextToMutate + numThisTime);  // advance to next one
    } else if (style.adjacentStyle == 2) {
        // assert(false);
        for (int i = 0; i < numThisTime; ++i) {
            for (int tries = 0; tries < 50; ++tries) {
                assert(tries < 48);
                //   const float rand = double(state.random()) * double(numBest - 1) / (std::numeric_limits<uint64_t>::max());

                // generate a new random index
              //  const int candidateIndex = double(state.random()) * double(row.getSize() - 1) / (std::numeric_limits<uint64_t>::max());
                const int candidateIndex = 123456;
                
          SQINFO("generated rand for rowsize %d = %d",(int) row.getSize(), candidateIndex );
                // but don't let it duplicate one we have
                if (!toMutateIncludes(indiciesToMutate, candidateIndex)) {
                    indiciesToMutate[index++] = candidateIndex;  // add it if it's good
                    indiciesToMutate[index] = -1;        // and keep the terminator up to date
                    break;                                  // and leave this inner loop
                }
            }
        }
    } else {
        assert(style.adjacentStyle == 0);
        const double quota = double(row.getSize()) / double(numThisTime);
        double floatingAcc = 0;
        // int integerAcc = 0;

        // SQINFO("non adj. size=%lld num=%d q=%f", row.getSize(), numThisTime, quota);
        indiciesToMutate[index++] = state.nextToMutate;  // always mutate the current one
        for (size_t i = 1; i < row.getSize(); ++i) {
            floatingAcc += 1.0;

            // SQINFO("i=%lld facc=%f", i, floatingAcc);
            if (floatingAcc >= quota) {
                const int x = row.wrapIndex(i + state.nextToMutate);
                indiciesToMutate[index++] = x;
                floatingAcc -= std::floor(floatingAcc);
            }
        }
        state.nextToMutate = row.wrapIndex(state.nextToMutate + 1);  // advance to next one
        // SQINFO("advance next 117 to %d", state.nextToMutate);
    }

    indiciesToMutate[index] = -1;
}

void MelodyGenerator::_mutateSome(MelodyRow& row, const Scale& scale, MelodyMutateState& state, const MelodyMutateStyle& style, int* indiciesToMutate) {
    for (int i = 0; indiciesToMutate[i] >= 0; ++i) {
        const int index = indiciesToMutate[i];
        _mutateOne(row, index, scale, state, style);
    }
}

void MelodyGenerator::_mutateOne(MelodyRow& row, size_t noteIndex, const Scale& scale, MelodyMutateState& state, const MelodyMutateStyle& style) {
    assert(style.keepInScale);  // don't know how to do other.
    // assert(style.roundRobin);
    assert(style.adjacentStyle == 0);
    assert(noteIndex < row.getSize());

    // SQINFO("mutateOne %d", (int)noteIndex);

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