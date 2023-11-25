
#pragma once

#include <assert.h>

#include <vector>

#include "Chord4.h"

class Chord4List {
public:
    Chord4List(const Options& options, int root, bool show=false);

    int size() const;  // how many chords are in list

    // If there is an error constructing chords, this is how we signal it.
    bool isValid() const { return !chords.empty(); }

    const Chord4* get2(int n) const;

private:
    std::vector<Chord4Ptr> chords;
};

inline int Chord4List::size() const {
    return int(chords.size());
}

inline const Chord4* Chord4List::get2(int n) const {
    assert(isValid());
    if (!isValid()) {
        return nullptr;
    }
    // SQINFO("Chord4List::get2 n=%d size=%d", n, int(size()));
    if (n >= size()) {
        return nullptr;
    }
    assert(n < size());
    return chords[n].get();
}
using Chord4ListPtr = std::shared_ptr<Chord4List>;
