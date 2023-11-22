
#include "Chord4List.h"

#include <algorithm>

#include "Options.h"
#include "SqLog.h"

static int compareChords(const Options& options, Chord4Ptr ch1, Chord4Ptr ch2) {
    const int q1 = ch1->quality(options, false);
    const int q2 = ch2->quality(options, false);

    return q1 > q2;
}

Chord4List::Chord4List(const Options& options, int rt, bool show) : _show(show) {
    if (_show) SQINFO("Chord4List ctor");
    Chord4 referenceChord(options, rt);
    for (bool done = false; !done;) {
        Chord4Ptr newChord = std::make_shared<Chord4>();
        *newChord = referenceChord;
        if (!newChord->isValid()) {
            if (_show) SQINFO("Chord4List ctor return with error");
            chords.clear();
            assert(chords.empty());
            return;
        }

        chords.push_back(newChord);               // put a chord in the list
        //SQINFO("pushed another chord, now %d", chords.size());
        done = referenceChord.makeNext(options);  // advance to next chord
        if (_show) SQINFO("Chord4List ctor pushed another: %d", int(chords.size()));
    }

    assert(!chords.empty());  // in theory ok, but don't know if we handle it.
    if (_show) SQINFO("now will sort %d", int(chords.size()));
    std::sort(chords.begin(), chords.end(), [options](Chord4Ptr c1, Chord4Ptr c2) {
        return compareChords(options, c1, c2);
    });

    // Now that we are sorted, add the ranks
    int rank = 0;
    for (auto it : chords) {
        it->rank = rank++;
    }
}
