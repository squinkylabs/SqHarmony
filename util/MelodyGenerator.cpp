#include "MelodyGenerator.h"

#include <algorithm>

#include "MelodyEvaluator.h"
#include "NoteConvert.h"
#include "PitchKnowledge.h"
#include "EvaluationSummary.h"

bool verboseProbability = false;

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

void MelodyGenerator::makeStateLegal(MelodyMutateState& state, const MelodyRow& row) {
    if (state.nextToMutate >= row.getSize()) {
        state.nextToMutate = 0;
    }
}

EvaluationSummary MelodyGenerator::mutate(MelodyRow& row, MelodyMutateState& state, const MelodyMutateStyle& style) {
    makeStateLegal(state, row);
    assert(row.getSize() <= 16);

    // special case for only one. It's a common case, and faster.
    if (style.numToMutate == 1) {
        const size_t noteIndex = state.nextToMutate;
        assert(style.numToMutate == 1);
        _mutateOne(row, noteIndex, state, style);
        state.nextToMutate = row.wrapIndex(state.nextToMutate + 1);
        // SQINFO("in mutate: advance next 65 to %d", state.nextToMutate);
        return EvaluationSummary();
    }

    int toMutate[16 + 1];
    getIndiciesToMutate(row, state, style, toMutate);
    // SQINFO("to mutate = %d %d %d %d %d", toMutate[0], toMutate[1], toMutate[2], toMutate[3], toMutate[4]);
    return _mutateSome(row, state, style, toMutate);
}

bool MelodyGenerator::toMutateIncludes(const int* indiciesToMutate, int candidateIndex) {
    assert(candidateIndex >= 0);
    for (const int* p = indiciesToMutate;; ++p) {
        const int x = *p;
        if (x < 0) {
            return false;
        }
        if (x == candidateIndex) {
            return true;
        }
    }
}

/**
 * perform the "distribute" part of determining indicies to mutate.
 * parameter list is a little weird since this was removed from another function
 */
static void distribute(MelodyRow& row, int numThisTime, int* indiciesToMutate, int& index, int startingIndex) {
    const double quota = double(row.getSize() / double(numThisTime));

    // SQINFO("non adj. size=%lld num=%d q=%f", row.getSize(), numThisTime, quota);
    double floatingAcc = 0;
    indiciesToMutate[index++] = startingIndex;  // state.nextToMutate;  // always mutate the current one
    for (size_t i = 1; i < row.getSize(); ++i) {
        floatingAcc += 1.0;

        // SQINFO("i=%lld facc=%f", i, floatingAcc);
        if (floatingAcc >= quota) {
            const int x = row.wrapIndex(i + startingIndex);
            indiciesToMutate[index++] = x;
            floatingAcc -= std::floor(floatingAcc);
        }
    }
    //  state.nextToMutate = row.wrapIndex(state.nextToMutate + 1);  // advance to next one
}

void MelodyGenerator::getIndiciesToMutate(MelodyRow& row, MelodyMutateState& state, const MelodyMutateStyle& style, int* indiciesToMutate) {
    const int numThisTime = std::min(row.getSize(), size_t(style.numToMutate));
    int index = 0;
    indiciesToMutate[0] = -1;  // init to zero length.
    if (style.numToMutate == 0) {
        size_t i;
        for (i = 0; i < row.getSize(); ++i) {
            indiciesToMutate[i] = i;
        }
        index = i;  // so that at the end we can terminate
    } else if (style.slotSelectionMethod == SlotSelectionMethod::ROUND_ROBIN_ADJACENT) {
        for (int i = 0; i < numThisTime; ++i) {
            int x = state.nextToMutate + i;
            // SQINFO("use at 90  %d", state.nextToMutate);
            x = row.wrapIndex(x);
            assert(x < (int)row.getSize());
            indiciesToMutate[index++] = x;
        }
        state.nextToMutate = row.wrapIndex(state.nextToMutate + numThisTime);  // advance to next one
    } else if (style.slotSelectionMethod == SlotSelectionMethod::RANDOM_RANDOM) {
        // assert(false);
        for (int i = 0; i < numThisTime; ++i) {
            for (int tries = 0; tries < 50; ++tries) {
                assert(tries < 48);
                const int candidateIndex = state.random.generateInteger(row.getSize());

                // but don't let it duplicate one we have
                if (!toMutateIncludes(indiciesToMutate, candidateIndex)) {
                    indiciesToMutate[index++] = candidateIndex;  // add it if it's good
                    indiciesToMutate[index] = -1;                // and keep the terminator up to date
                    break;                                       // and leave this inner loop
                }
            }
        }
    } else if (style.slotSelectionMethod == SlotSelectionMethod::RANDOM_ADJACENT) {
        int x = state.random.generateInteger(row.getSize());
        indiciesToMutate[index++] = x;
        for (int i = 1; i < numThisTime; ++i) {
            ++x;
            x = row.wrapIndex(x);
            indiciesToMutate[index++] = x;
        }
    } else if (style.slotSelectionMethod == SlotSelectionMethod::ROUND_ROBIN_DISTRIBUTED) {
        distribute(row, numThisTime, indiciesToMutate, index, state.nextToMutate);
        state.nextToMutate = row.wrapIndex(state.nextToMutate + 1);  // advance to next one
    } else if (style.slotSelectionMethod == SlotSelectionMethod::RANDOM_DISTRIBUTED) {
        const int nextToMutate = state.random.generateInteger(row.getSize());
        distribute(row, numThisTime, indiciesToMutate, index, nextToMutate);
    } else {
        assert(false);
    }
    indiciesToMutate[index] = -1;
}

EvaluationSummary MelodyGenerator::_mutateSome(MelodyRow& row, MelodyMutateState& state, const MelodyMutateStyle& style, int* indiciesToMutate) {
    for (int i = 0; indiciesToMutate[i] >= 0; ++i) {
        const int index = indiciesToMutate[i];
        _mutateOne(row, index, state, style);
    }

    assert(false);
    return EvaluationSummary();
}

void MelodyGenerator::_penalties2Probabilities(unsigned num, const float* penalties, float* probabilities) {
    float sum = 0;
    float maxPenalty = 0;
    for (unsigned i = 0; i < num; ++i) {
        if (verboseProbability) SQINFO("penalty[%d] = %f", i, penalties[i]);
        maxPenalty = std::max(maxPenalty, penalties[i]);
    }

    // now map from penalties to goodness
    for (unsigned i = 0; i < num; ++i) {
        probabilities[i] = maxPenalty - penalties[i];
        // float x = maxPenalty - penalties[i];
        // SQINFO("x = %f", x);
        // x = std::max(x, MelodyEvaluator::minProbability);
        // probabilities[i] = x;
        sum += probabilities[i];
        if (verboseProbability) SQINFO("proba[%d] = %f", i, probabilities[i]);
    }

    // special case for all the same
    if (sum == 0) {
        float p = 1.f / float(num);
        for (unsigned i = 0; i < num; ++i) {
            probabilities[i] = p;
        }
        SQINFO("special case all the same");
        return;
    }

    // find biggest
    float biggestP = 0;
    for (unsigned i = 0; i < num; ++i) {
        biggestP = std::max(biggestP, probabilities[i]);
    }

    float minP = biggestP * MelodyEvaluator::probabilityDynamicRange;
    //SQINFO("biggest = %f minP = %f", biggestP, minP);

    // Apply probability dynamic range
    sum = 0;
    for (unsigned i = 0; i < num; ++i) {
        probabilities[i] = std::max(probabilities[i], minP);
        sum += probabilities[i];
        if (verboseProbability) SQINFO("prob c = %f", probabilities[i]);
    }
      // Scale sum to one again
    assert(sum > 0);
    for (unsigned i = 0; i < num; ++i) {
        probabilities[i] *= (1.f / sum);
        if (verboseProbability) SQINFO("prob d = %f", probabilities[i]);
    }

}

EvaluationSummary MelodyGenerator::_mutateOne(MelodyRow& row, size_t noteIndex, MelodyMutateState& state, const MelodyMutateStyle& style) {
    assert(style.keepInScale);  // don't know how to do other.
    assert(noteIndex < row.getSize());

    // SQINFO("mutateOne %d", (int)noteIndex);

    int candidateShifts[] = {-2, -1, 1, 2, 0};
    int numCandidates = 0;
    for (int i = 0; 0 != candidateShifts[i]; ++i) {
        ++numCandidates;
    }
    MelodyRow mutatedCandidates[4];
    float penalties[4];
    // float lowestPenalty = 1000;  // insanely high penalty

    // First, make all the mutation candidates and get their penalties
    for (int i = 0; 0 != candidateShifts[i]; ++i) {
        mutatedCandidates[i] = row;
        _changeOneNoteInMode(mutatedCandidates[i], style.scale, noteIndex, candidateShifts[i]);
        const float penalty = MelodyEvaluator::getPenalty(mutatedCandidates[i], style);
        penalties[i] = penalty;
        //  lowestPenalty = std::min(penalty, lowestPenalty);

        if (verboseProbability) SQINFO("candidate[%d], penalty=%f data=%s", i, penalty, mutatedCandidates[i].toString().c_str());
    }

    assert(numCandidates == 4);
    float probabilities[4];
    _penalties2Probabilities(numCandidates, penalties, probabilities);

#if 0
    for (int i = 0; i < 4; ++i) {
        SQINFO("prob = %f, %f, %f, %f",
               probabilities[0],
               probabilities[1],
               probabilities[2],
               probabilities[3]);
    }
#endif

    // Stack the probabilities so we can evaluate.
    for (int i = 1; i < 4; ++i) {
        probabilities[i] += probabilities[i - 1];
    }

    // for (int i = 0; i < 4; ++i) {
    //     SQINFO("adjusted prob = %f", probabilities[i]);
    // }

    const float rand = state.random.generateDouble();
    //  SQINFO("rand = %f\n", rand);
    int chosenIndex = 0;
    for (int i = 0; i < 4; ++i) {
        if (probabilities[i] >= rand) {
            //  SQINFO("prob %d can fire\n", i);
            chosenIndex = i;
            break;
        }
    }
    //    SQINFO("chosen index = %d", chosenIndex);
    row = mutatedCandidates[chosenIndex];
    SQINFO("%s", MelodyEvaluator::toString(row, style).c_str());

    assert(false);
    return EvaluationSummary();
}

#if 0  // old way
void MelodyGenerator::_mutateOne(MelodyRow& row, size_t noteIndex, const Scale& scale, MelodyMutateState& state, const MelodyMutateStyle& style) {
    assert(style.keepInScale);  // don't know how to do other.
    assert(style.slotSelectionMethod == 0);
    assert(noteIndex < row.getSize());

    // SQINFO("mutateOne %d", (int)noteIndex);

    int candidateShifts[] = {-2, -1, 1, 2, 0};
    MelodyRow mutatedCandidates[4];
    float penalties[4];
    float lowestPenalty = 1000;  // insanely high penalty

    // First, make all the mutation candidates.
    for (int i = 0; 0 != candidateShifts[i]; ++i) {
        mutatedCandidates[i] = row;
        _changeOneNoteInMode(mutatedCandidates[i], scale, noteIndex, candidateShifts[i]);
        const float penalty = MelodyEvaluator::getPenalty(mutatedCandidates[i], style);
        penalties[i] = penalty;
        lowestPenalty = std::min(penalty, lowestPenalty);

        SQINFO("candidate[%d], penalty=%f lowest=%f", i, penalty, lowestPenalty);
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
    const int theRow = _pickOne(index, bestCandidates, state);
    row = mutatedCandidates[theRow];

    // SQINFO("%s", MelodyEvaluator::toString(row, style).c_str());
}
#endif

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