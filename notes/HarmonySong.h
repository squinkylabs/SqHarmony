
#pragma once

#include <memory>
#include <vector>

#include "RankedChord.h"

/**
 * @brief This guy is designed to write whole songs.
 *
 * It knows how to find a legal sequence of chords, given the roots.
 * It knows how to search for an optimal song, using branch and bound.
 */
class HarmonySong_unused {
    friend class HarmonySong2SequencerTrack;

public:
    HarmonySong_unused(const Options&, const int*);  // pass the progression this way

    void print() const;
    std::shared_ptr<RankedChord_unused> get(int n) {
        assert(n < int(chords.size()));
        return chords[n];
    }
    int size() const {
        return int(chords.size());
    }
    bool Generate(const Options& options, int Nlevel, bool show, PAStats* stats);  // Ret true if ok!
private:
    // the final chords we make
    std::vector<std::shared_ptr<RankedChord_unused>> chords;

    Chord4Manager chordManager;

    bool firstTime = true;
    bool isValid() const;
    void analyze(const Options& options) const;
};
