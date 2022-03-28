
#include "Chord4List.h"
#include "Options.h"
#include <algorithm>

#include "SqLog.h"

static int compareChords(const Options& options, Chord4Ptr ch1, Chord4Ptr ch2) {

    const int q1 = ch1->quality(options, false);
    const int q2 = ch2->quality(options, false);

    return q1 > q2;
}

Chord4List::Chord4List(const Options& options, int rt) {
    Chord4 referenceChord(options, rt);
    for (bool done=false; !done; ) {
        Chord4Ptr newChord = std::make_shared<Chord4>();
        *newChord = referenceChord;
        if (!newChord->isValid()) {
            chords.clear();
            assert(chords.empty());
            return;
        }
        chords.push_back(newChord);   // put a chord in the list
        done = referenceChord.makeNext(options);  // advance to next chord
    }

    assert(!chords.empty());        // in theory ok, but don't know if we handle it.
    std::sort(chords.begin(), chords.end(), [options](Chord4Ptr  c1, Chord4Ptr  c2) {
            return compareChords(options, c1, c2);
    });

    int rank=0;
    for (auto it : chords) {
        it->rank = rank++;
    }
}
