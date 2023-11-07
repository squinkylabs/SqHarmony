#pragma once

#include <vector>

#include "Chord4.h"
#include "Chord4List.h"

/**
 * @brief holds all the chords for the scale we are in
 * 
 */

using Chord4ListPtr = std::shared_ptr<Chord4List>;
class Chord4Manager {
public:
    Chord4Manager(const Options& options) {
        for (int i = 0; i < 10; ++i) {
            if (i > 0 && i < 8) {
                auto newChord = std::make_shared<Chord4List>(options, i);
                if (!newChord->isValid()) {
                    chords.clear();
                    assert(chords.empty());
                    assert(!isValid());
                    // SQINFO("chord4manager init failed");
                    return;
                }
                chords.push_back(newChord);
            } else {
                chords.push_back(nullptr);
            }
        }
    }

    bool isValid() const { return !chords.empty(); }
    int size(int root) const {
        assert(root > 0);
        assert(chords.size() == 10);
        return chords[root]->size();
    }

    // same as get, but can return "not found" (nullptr)
    const Chord4* get2(int root, int rank) const {
        assert(isValid());
        if (!isValid()) {
            return nullptr;
        }
        if (root >= int(chords.size())) {
            return nullptr;
        }

        return chords[root]->get2(rank);
    }
    
    // only for unit test?
    Chord4List* _getChords(int sourceRoot) {
       // assert(false);
       // return nullptr;
       const Chord4ListPtr x = chords[sourceRoot];
       return x.get();
    }



    int _size() const {
        return chords[1]->size();
    }

private:
    // entries for 0 = no=used, 1= root
    std::vector<Chord4ListPtr> chords;
};

using Chord4ManagerPtr = std::shared_ptr<Chord4Manager>;
