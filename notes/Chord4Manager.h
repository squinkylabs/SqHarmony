#pragma once

#include "Chord4.h"
#include "Chord4List.h"

#include <vector>

using Chord4ListPtr = std::shared_ptr<Chord4List>;
class Chord4Manager {
public:
    Chord4Manager(const Options& options) {
        for (int i = 0; i < 10; ++i) {
            //Chord4 ch(i);
           // Chord4Ptr chp = std::make_shared<Chord4>(i);
            chords.push_back( std::make_shared<Chord4List>(options, i));
        }
    }
    int size(int root) const {
        assert(chords.size() == 10);
        return chords[root]->size();
    }
#if 0 // dangerous
    const Chord4& get(int root, int rank) const {
        assert(!chords.empty());
        assert (root < int(chords.size()));
        return chords[root]->get(rank);
    }
#endif

    // same as get, but can return "not found" (nullptr)
    const Chord4* get2(int root, int rank) const {
        assert(!chords.empty());
        if (root >= int(chords.size())) {
            return nullptr;
        }

        return chords[root]->get2(rank);
    }

    int _size() const {
        return chords[1]->size();
    }

private:
    // entries for 0 = no=used, 1= root
    // Chord4Ptr p;
    std::vector<Chord4ListPtr> chords;
};

using Chord4ManagerPtr = std::shared_ptr<Chord4Manager>;

