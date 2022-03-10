
#pragma once

#include <assert.h>

#include <vector>

#include "Chord4.h"

class Chord4List {
public:
    Chord4List(const Options& options, int root);
    int size() const;  // how many chords are in list

    // this version is dangerous, as destructor of chord can get called
    // const Chord4& get(int n) const;  // Get chord number n
    // could return Chord4Ptr....
    const Chord4* get2(int n) const;

private:
    // int nElements;
    // Chord4* chordList;
    std::vector<Chord4Ptr> chords;
};

inline int Chord4List::size() const {
    return chords.size();
}

#if 0
inline const Chord4& Chord4List::get(int n) const {
    assert(n < size());
    return *chords[n];
}
#endif

inline const Chord4* Chord4List::get2(int n) const {
    assert(n < size());
    return chords[n].get();
}
using Chord4ListPtr = std::shared_ptr<Chord4List>;
