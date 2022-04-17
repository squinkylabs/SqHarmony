#pragma once

#include <set>

class RefMonitor {
public:
    RefMonitor() {
        for (int i=0; i<256; ++i) {
            buffer[i] = (i == index) ? 0 : 0xaa55;
        }
    }
    int get() const { return buffer[index]; }
    void dump() const {
        SQINFO("--- dumping buffer ");
        for (int i = 0; i < 256; ++i) {
            int value = buffer[i];
            if ((i != index) && (value != 0xaa55)) {
                SQINFO("bad padding buffer[%d] = %d", i, buffer[i]);
            }
            if (i == index) {
                 SQINFO("bad count buffer[%d] = %d", i, buffer[i]);
            }
        }
    }

    void validate(const Chord4* ch) const {
        if (!haveEverSeen(ch)) {
            SQINFO("can't validate unknown chord %p");
        }
        assert(haveSeen(ch));
    }

    void increment(const Chord4* ch) { 
        buffer[index]++;
        assert(!haveSeen(ch));
        allocations.insert(ch);
        all.insert(ch);
    }
    void decrement(const Chord4* ch) {
      
        if (!haveSeen(ch)) {
            SQINFO("!!!! dec one we didn't allocate have ever=%d", haveEverSeen(ch));
        }
        assert(haveSeen(ch));
        allocations.erase(ch);
        buffer[index]--;
        bool corrupt = false;
        for (int i=0; i<256; ++i) {
            if ((i != index) && (buffer[i] != 0xaa55)) {
                SQINFO("memory corrupt at index%d = %x", i, buffer[i]);
                corrupt = true;
            }
        }
        if (corrupt) {
            SQINFO("count is %d", buffer[index]);
        }
        assert(!corrupt);
        if (buffer[index] < 0) {
            SQINFO("!!!! ref count underflow to %d", buffer[index]);
            dump();

           // assert(false);
        }
    }

private:
    const int index = 100;
    int buffer[256] = {};
    std::set<const Chord4 *> allocations;
    std::set<const Chord4 *> all;

    bool haveSeen(const Chord4* ch) const {
        return allocations.find(ch) != allocations.end();
    }
     bool haveEverSeen(const Chord4* ch) const {
        return all.find(ch) != all.end();
    }
};