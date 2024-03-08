
#pragma once
#include <assert.h>

#include <memory>
#include <vector>

#include "Chord4.h"
#include "Chord4List.h"
#include "Chord4Manager.h"

class RankedChord_unused {
public:
    RankedChord_unused(const Chord4Manager&, int rt);
    ~RankedChord_unused();
    bool makeNext();  // advance to next worst chord
    void reset();     // set us back to the first chord in rank
    const Chord4* fetch2() const;
    void print() const;  // print the current chord
    int penaltyForFollowingThisGuy(const Options&, int lowerBound, const RankedChord_unused& ThisGuy, bool show, PAStats* stats) const;

private:
    const Chord4Manager& chords;
    int curRank;  // the current chord
    const int root;
};

inline RankedChord_unused::RankedChord_unused(const Chord4Manager& mgr, int rt) : chords(mgr), root(rt) {
    curRank = 0;
}

inline RankedChord_unused::~RankedChord_unused() {
}

inline int RankedChord_unused::penaltyForFollowingThisGuy(const Options& options, int upperBound, const RankedChord_unused& theGuy, bool show, PAStats* stats) const {
    // fetch made safer
    return fetch2()->penaltForFollowingThisGuy(options, upperBound, theGuy.fetch2(), show, stats);
}

inline void RankedChord_unused::reset() {
    curRank = 0;
}

inline bool RankedChord_unused::makeNext() {
    bool ret;
    const int size = chords.size(root);
    assert(size >= 1);
    if (curRank >= (size - 1)) {
        ret = true;
    } else {
        ret = false;
        curRank++;
    }
    return ret;
}

inline const Chord4* RankedChord_unused::fetch2() const {
    return chords.get2(root, curRank);
}

inline void RankedChord_unused::print() const {
    printf("rank:%3d ", curRank);
    fetch2()->print();
}
