
#pragma once
#include <assert.h>

#include <memory>
#include <vector>

#include "Chord4.h"
#include "Chord4List.h"

#include "Chord4Manager.h"

#if 0
class ChordListWrapper {
public:
    ChordListWrapper() {
        Ref = 0;
        pList = 0;
    }
    operator const Chord4List&() const { return *pList; }
    void Add(int root);
    void Del(int root);

private:
    int Ref;
    Chord4List* pList;
};

inline void ChordListWrapper::Add(int root) {
    if (!Ref)  // if we haven't make a clist for this guy yet
    {
        assert(!pList);
        pList = new Chord4List(root);
    }
    Ref++;
}

inline void ChordListWrapper::Del(int root) {
    assert(pList);
    if (Ref == 1) {
        delete pList;
        pList = 0;
    }
    Ref--;
    assert(Ref >= 0);
}

class ChordListSharerArray {
public:
    void Add(int root) { CLWarray[root].Add(root); }
    void Del(int root) { CLWarray[root].Del(root); }
    const Chord4List& operator[](int x) const { return CLWarray[x]; }

private:
    static ChordListWrapper CLWarray[9];  // global lists for all possible chords
};
#endif

class RankedChord {
public:
    RankedChord(const Chord4Manager&, int rt);
    ~RankedChord();
    bool makeNext();              // advance to next worst chord
    void reset();                 // set us back to the first chord in rank
    const Chord4& fetch() const;  // get the current chord
    void print() const;           // print the current chord
  //  bool canFollowThisGuy(const Options& options, const RankedChord& TheGuy);
    int penaltyForFollowingThisGuy(const Options&, int lowerBound, const RankedChord& ThisGuy, bool show) const;

private:
    const Chord4Manager& chords;
    int curRank;  // the current chord
    const int root;
};

inline RankedChord::RankedChord(const Chord4Manager& mgr , int rt) : chords(mgr), root(rt) {
    curRank = 0;
}

inline RankedChord::~RankedChord() {
  //  ChordArray.Del(root);
}

inline int RankedChord::penaltyForFollowingThisGuy(const Options& options, int upperBound, const RankedChord& theGuy, bool show) const {
    return fetch().penaltForFollowingThisGuy(options, upperBound, theGuy.fetch(), show);
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

inline const Chord4& RankedChord::fetch() const {
    
  //  const int size = chords.size(root);
   // printf("RankedChord fetch size = %d, curRank = %d\n", size, CurRank);

  //  return ChordArray[root].get(CurRank);
    return chords.get(root, curRank);
    //return *ChordArray[root][CurRank];
}

inline void RankedChord::print() const {
    printf("rank:%3d ", curRank);
    fetch().print();
}
