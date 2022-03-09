
#pragma once

#include "Chord4.h"

#include <assert.h>
#include <vector>

class Chord4List {
public:
    Chord4List(const Options& options, int root);
    int size() const;               // how many chords are in list
    const Chord4& get(int n) const;  // Get chord number n

private:
    //int nElements;
    //Chord4* chordList;
    std::vector<Chord4Ptr> chords;
};

inline int Chord4List::size() const {
    return chords.size();
}

inline const Chord4& Chord4List::get(int n) const {
    assert(n < size());
    return *chords[n];
}

 using Chord4ListPtr = std::shared_ptr<Chord4List>;
