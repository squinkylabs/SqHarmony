
#include "Chord4List.h"

#include <algorithm>

#include "Options.h"
#include "RawChordGenerator.h"
#include "SqLog.h"

static int compareChords(const Options& options, Chord4Ptr ch1, Chord4Ptr ch2) {
    const int q1 = ch1->quality(options, false);
    const int q2 = ch2->quality(options, false);

    return q1 > q2;
}

int good = 0;
int bad = 0;

#if _CHORD4_USE_NEW == true
Chord4List::Chord4List(const Options& options, int root, bool show) : _show(show) {
    RawChordGenerator rawGen(options, root);
    Chord4 refChord;
    for (bool done = false; !done;) {
        const bool b = rawGen.getNextChord();
        if (!b) {
            done = true;
        } else {
            int chord[4];
            rawGen.getCurrentChord(chord);
            assert(chord[3] > 0);
        //    const auto newChord = std::make_shared<Chord4>(options, rt, chord, show);
         //  a b 
            Chord4* pchord = new (&refChord) Chord4(options, root, chord, show);
            __numChord4--;  // this one doesn't count for ref-counting
           // new (refChord) Chord4(options, rt, chord, show);
           // const int r = newChord->fetchRoot();
         //   const auto name = newChord->toStringShort();

            if (!pchord->isValid() || !pchord->isChordOk(options)) {
                pchord->isChordOk(options);
                //    assert(newChord->isValid());
                //     SQINFO("not pushing bad chord");
                bad++;
            } else {
                Chord4Ptr newChord = std::make_shared<Chord4>(options, root, chord, show);
                chords.push_back(newChord);
                good++;
            }
        }
    }
    SQINFO("good/bad = %f total=%d", double(good) / double(bad), good + bad);
}
#else
Chord4List::Chord4List(const Options& options, int rt, bool show) : _show(show) {
    if (_show) SQINFO("Chord4List ctor");
    Chord4 referenceChord(options, rt, true);
    assert(referenceChord.isValid());
    assert(referenceChord.isChordOk(options));
    for (bool done = false; !done;) {
        Chord4Ptr newChord = std::make_shared<Chord4>();
        *newChord = referenceChord;
        if (!newChord->isValid()) {
            if (_show) SQINFO("Chord4List ctor return with error");
            chords.clear();
            assert(chords.empty());
            return;
        }

        chords.push_back(newChord);  // put a chord in the list
        // SQINFO("pushed another chord, now %d", chords.size());
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
#endif


Chord4Ptr Chord4List::fromString(const Options& options, int degree, const char* string) {
    //  Chord4List(const Options& options, int root, bool show = false);
    Chord4List chords(options, degree);
    std::string str(string);
    for (int i = 0; i < chords.size(); ++i) {
        const Chord4* chord = chords.get2(i);
        if (str == chord->toStringShort()) {
            Chord4Ptr ret = std::make_shared<Chord4>();
          //  Chord4* pdest = ret.get();
           
          //  (*pdest) = (*chord);
            *ret = *chord;
            return ret;
        }
    }
    assert(false);
    return nullptr;
}
 