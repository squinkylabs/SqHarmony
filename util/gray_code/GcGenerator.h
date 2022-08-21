#pragma once

#include <vector>

#include "GcUtils.h"
class GcGenerator {
public:
    using NumType = uint16_t;  // let's limit ourselves to 16 bit
    GcGenerator(unsigned nBits, bool balanced);
    void dump();

private:
    //  bool addAnotherEntry();
    bool addTheRemainingEntries(int marker);
    // bool canThisFollow(NumType candidate) const;
    bool isThisStateValid() const;
    std::vector<NumType> state;
    //  std::vector<NumType> data;  // the generated dat
    const unsigned nBits;
    unsigned numEntries;
    const bool wantBalanced;
    unsigned farthestBacktrack = 1000;

    unsigned getCandidate(int bit);
};

inline GcGenerator::GcGenerator(unsigned nBits, bool balanced) : nBits(nBits), wantBalanced(balanced) {
    numEntries = unsigned(std::round(std::pow<int>(2, nBits)));
    printf("gcgen ctor, nBits=%d numEnt=%d\n", nBits, numEntries);

    const auto success = addTheRemainingEntries(123);
    assert(success);
    assert(state.size() == numEntries);
    // data = state;
}

//  GcUtils::onlyDifferByOneBit(unsigned totalCount, unsigned numBits, const std::vector<uint16_t>& data1, unsigned candidate)

inline bool GcGenerator::isThisStateValid() const {
    //  SQINFO("can this follow (%d) size =%d", candidate, state.size());
    printf("isThisStateValid called on:\n");
    GcUtils::dumpBinary(nBits, state);

    // if we are the first guess, we are fine
    if (state.size() < 2) {
        return true;
    }

    const auto differByOne = GcUtils::onlyDifferByOneBit(numEntries, nBits, state);
    if (!differByOne) {
        printf("not valid, differ by one failed\n");
        return false;
    }

    const auto candidate = state.back();
    for (unsigned i = 0; i < state.size() - 1; ++i) {
        const auto x = state[i];
        if (x == candidate) {
             printf("not valid, two entries same: %x, %x\n", x, candidate);
            return false;
        }
    }

    return true;
}

inline unsigned GcGenerator::getCandidate(int bit) {
    if (state.empty()) {
        return 0;  // always start at zero
    }
    const auto last = state.back();
    const auto mask = 1 << bit;
    return last ^ mask;
}

inline bool GcGenerator::addTheRemainingEntries(int marker) {
    printf("addAnotherENtry called size=%zd\n", state.size());
    // first, make next entry

    // TODO: we don't need a special case here?
    if (state.size() == numEntries) {
        if (!wantBalanced) {
            return true;
        }
        const auto dist = GcUtils::getTranstionDataWithWrap(nBits, state);  // this won't look for round abouts!
        const bool isBalanced = dist.first == dist.second;
        if (!isBalanced) {
            printf("\not balanced, dist = %d,%d\n", dist.first, dist.second);
            // dump();
            return false;
        }
        // if they are all made, we are good

        return true;
    }

    // loop, searching for something to try.
    for (unsigned i = 0; i < nBits; ++i) {
       
        const auto candidate = getCandidate(i);
         printf("calling get Candidate from loop i=%d level=%zd cand=%x\n", i, state.size(), candidate);
        state.push_back(candidate);
        if (isThisStateValid()) {
            printf("state valid, size=%zd\n", state.size());
            const bool ok = addTheRemainingEntries(678);
            if (ok) {
                //  SQINFO("that succeeded\n");
                return true;
            } else {
                // SQINFO("guess failed %x level = %d i=%d", candidate, state.size(), i);
                assert(!state.empty());
                state.pop_back();  // pop off the failed candidate

                if (state.size() < farthestBacktrack) {
                    farthestBacktrack = unsigned(state.size());
                    printf("backtrack to %d\n", farthestBacktrack);
                }
            }
        } else {
            printf("state bad, size=%zd\n", state.size());
            state.pop_back();
        }
        /*
        if (canThisFollow(candidate)) {
            state.push_back(candidate);
            // if this candidate looks possible, let's recurse into it
            const bool ok = addTheRemainingEntries(678);
            if (ok) {
                //  SQINFO("that succeeded\n");
                return true;
            } else {
                //SQINFO("guess failed %x level = %d i=%d", candidate, state.size(), i);
                assert(!state.empty());
                state.pop_back();  // pop off the failed candidate

                if (state.size() < farthestBacktrack) {
                    farthestBacktrack = unsigned(state.size());
                    SQINFO("backtrack to %d", farthestBacktrack);
                }
            }
        }
        */
    }

    // if we got here, this whole stage can't complete.
    if (state.empty()) {
        printf("giving up on level %zd marker = %d", state.size(), marker);
    } else {
        printf("giving up on level %zd marker = %d top=%x", state.size(), marker, state.back());
    }
    // assert(!state.empty());
    //  state.pop_back();
    return false;
}

inline void GcGenerator::dump() {
    printf("\n");
    for (unsigned i = 0; i < numEntries; ++i) {
        GcUtils::dumpBinary(nBits, state[i]);
        printf(" hex=%x", state[i]);
        printf("\n");

        // SQINFO("%x", state[i]);
    }
    const auto x = GcUtils::getTranstionDataWithWrap(nBits, state);
    printf("want bal =%d, var=%d,%d\n", wantBalanced, x.first, x.second);
    printf("shortest run = %d\n", GcUtils::getShortestRun(nBits, state));
    printf("num bits=%d stateSize = %zd\n", nBits, state.size());
}
