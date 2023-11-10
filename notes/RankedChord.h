
#pragma once
#include <assert.h>

#include <memory>
#include <vector>

#include "Chord4.h"
#include "Chord4List.h"
#include "Chord4Manager.h"

class RankedChord {
public:
    RankedChord(const Chord4Manager&, int rt);
    ~RankedChord();
    bool makeNext();  // advance to next worst chord
    void reset();     // set us back to the first chord in rank
    const Chord4* fetch2() const;
    void print() const;  // print the current chord
    int penaltyForFollowingThisGuy(const Options&, int lowerBound, const RankedChord& ThisGuy, bool show) const;

private:
    const Chord4Manager& chords;
    int curRank;  // the current chord
    const int root;
};

inline RankedChord::RankedChord(const Chord4Manager& mgr, int rt) : chords(mgr), root(rt) {
    curRank = 0;
}

inline RankedChord::~RankedChord() {
}

inline int RankedChord::penaltyForFollowingThisGuy(const Options& options, int upperBound, const RankedChord& theGuy, bool show) const {
    // fetch made safer
    return fetch2()->penaltForFollowingThisGuy(options, upperBound, theGuy.fetch2(), show);
}

inline void RankedChord::reset() {
    curRank = 0;
}

inline bool RankedChord::makeNext() {
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

inline const Chord4* RankedChord::fetch2() const {
    return chords.get2(root, curRank);
}

inline void RankedChord::print() const {
    printf("rank:%3d ", curRank);
    fetch2()->print();
}
