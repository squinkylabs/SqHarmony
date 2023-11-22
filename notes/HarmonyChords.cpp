#include "HarmonyChords.h"

#include "Chord4Manager.h"
#include "ProgressionAnalyzer.h"

#if 0
const Chord4* HarmonyChords::findChord(
    bool show,
    const Options& options,
    const Chord4Manager& manager,
    int root,
    PAStats* stats) {
    return find(show, options, manager, nullptr, nullptr, root, nullptr, stats);
}
#endif

#if 1
const Chord4* HarmonyChords::findChord(
    bool show,
    const Options& options,
    const Chord4Manager& manager,
    int root,
    PAStats* stats) {

    SQINFO("does it still make sense to call this old code?");
   // assert(false);  // who is calling this other implementation of find??
    // SQINFO("enter HarmonyChords::findChord");
    assert(manager.isValid());
    // TODO assert that root is in scale (scale needs size())
    assert(root >= 1);
    assert(manager.isValid());

    // but now we find issues, so let's get the best root pos
    const int size = manager.size(root);
    int rankToTry = 0;
    for (bool done = false; !done; ++rankToTry) {
        if (rankToTry >= size) {
            return nullptr;
        }
        const Chord4* chord = manager.get2(root, rankToTry);
        assert(chord);
        assert(chord->isValid());
        // SQINFO("in find chord loop %s", chord->toString().c_str());

        // only accept a chord in root position with nice doubling
        if ((chord->inversion(options) == ROOT_POS_INVERSION) &&
            (chord->isCorrectDoubling(options))) {
            return chord;
        }
    }
    assert(false);
    return nullptr;
}
#endif

const Chord4* HarmonyChords::findChord(
    bool show,
    const Options& options,
    const Chord4Manager& manager,
    const Chord4& prev,
    int root,
    PAStats* stats) {
    return find(show, options, manager, nullptr, &prev, root, nullptr, stats);
}

const Chord4* HarmonyChords::findChord(
    bool show,
    const Options& options,
    const Chord4Manager& manager,
    const Chord4& prevPrev,
    const Chord4& prev,
    int root,
    PAStats* stats) {
    const Chord4* ret = find(show, options, manager, &prevPrev, &prev, root, nullptr, stats);
    assert(ret);  // we should always find something;
    return ret;
}

const Chord4* HarmonyChords::find(
    bool show,
    const Options& options,
    const Chord4Manager& manager,
    const Chord4* prevPrev,
    const Chord4* prev,
    int root,
    ChordHistory* history,
    PAStats* stats) {
    assert(root > 0);
    assert(root < 8);

    if (show) {
        SQINFO("in HarmonyChords::find");
    }

    //  SQINFO("\n---------------------- find pp=%p p=%p r = %d", prevPrev, prev, root);

#if 0
    if (prev && prevPrev) {
        printf("find called with prevOrev %s (root %d)\n", prevPrev->toString().c_str(), prevPrev->fetchRoot());
        printf("  .. prev %s (root %d)\n",
               prev->toString().c_str(),
               prev->fetchRoot());
        printf(" .. and cur root % d\n", root);
    } else if (prev) {
        printf("find called with prev %s (root %d) and cur root %d\n",
               prev->toString().c_str(),
               prev->fetchRoot(),
               root);
    }
#endif
    assert(manager.isValid());
    if (prev) assert(prev->isValid());
    if (prevPrev) assert(prevPrev->isValid());

    if (!prev) assert(!prevPrev);

    const int size = manager.size(root);
    int rankToTry = 0;
    // printf("in find, rank start = %d, size=%d\n", rankToTry, size);

    int lowestPenalty = ProgressionAnalyzer::MAX_PENALTY;
    const Chord4* bestChord = nullptr;

    for (bool done = false; !done; ++rankToTry) {
        if (rankToTry >= size) {
            if (show) SQINFO("rankToTry=%d size=%d done", rankToTry, size);
            done = true;
        } else {
            const Chord4* currentChord = manager.get2(root, rankToTry);
            bool isRepeat = false;
            if (history) {
                isRepeat = history->haveSeen(currentChord->rank, root);
                // SQINFO("isRepeat = %d", isRepeat);
            }

            // if bad repeating chord, don't evaluate, just give terrible scored
            const int currentPenalty = isRepeat ? ProgressionAnalyzer::MAX_PENALTY : progressionPenalty(options, lowestPenalty, prevPrev, prev, currentChord, show, stats);
            // SQINFO("curPen=%d", currentPenalty);
            if (currentPenalty == 0) {
                // printf("found penalty 0\n");
                if (history) {
                    history->onNewChord(currentChord->rank, root);
                }
                return currentChord;
            }
            if (show) SQINFO("hit a penalty in search %d", currentPenalty);
            if (currentPenalty < lowestPenalty) {
                lowestPenalty = currentPenalty;
                bestChord = currentChord;
            }
        }
    }
    // assert(bestChord);
    // printf("didn't find perfect, returning penalty = %d\n", lowestPenalty);
    if (history && bestChord) {
        // SQINFO("will add to hist, bc = %p", bestChord);
        history->onNewChord(bestChord->rank, root);
    }
    return bestChord;
}

const Chord4* HarmonyChords::findChord2(
    bool show,
    int root,
    const Options& options,
    const Chord4Manager& manager,
    ChordHistory* history,
    const Chord4* prevPrev,
    const Chord4* prev,
    PAStats* stats) {
    return find(show, options, manager, prevPrev, prev, root, history, stats);
}

int HarmonyChords::progressionPenalty(
    const Options& options,
    int bestSoFar,
    const Chord4* prevPrev,
    const Chord4* prev,
    const Chord4* current,
    bool show,
    PAStats* stats) {
    assert(current);

    if (!prevPrev && !prev) {
        return 0;  // chord on its own is always ok
    }

    int currentPenalty = current->penaltForFollowingThisGuy(options,
                                                            bestSoFar,
                                                            prev,
                                                            show,
                                                            stats);
    if (!prevPrev) {
        return currentPenalty;
    }

    if (currentPenalty > bestSoFar) {
        return currentPenalty;
    }

    const bool firstEqualsThird = (*current == *prevPrev);

    if (firstEqualsThird) {
#if 0
        printf("first equals third\n");
        printf("penalty %s\n", current->toString().c_str());
        printf("first = %s\n", prevPrev->toString().c_str());
        printf("second = %s\n", prev->toString().c_str());
#endif
        currentPenalty += ProgressionAnalyzer::PENALTY_FOR_REPEATED_CHORDS;
    }
    return currentPenalty;
}
