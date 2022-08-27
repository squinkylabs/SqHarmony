#pragma once

#include <algorithm>
#include <cmath>
#include <vector>

#include "GcUtils.h"
class GcGenerator {
public:
    using NumType = uint16_t;  // let's limit ourselves to 16 bit
    GcGenerator(unsigned nBits, bool wantBalanced, int minShortRun = -1);
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
    const int minShortRun;
    int shortestSeen = 1000;

    unsigned getCandidate(int bit);
    bool isFinalStateOk();
};

inline GcGenerator::GcGenerator(unsigned nBits, bool balanced, int minRun) : nBits(nBits), wantBalanced(balanced), minShortRun(minRun) {
    numEntries = unsigned(std::round(std::pow<int>(2, nBits)));
    printf("gcgen ctor, nBits=%d numEnt=%d\n", nBits, numEntries);
    printf(" minShortTurn=%d\n", minShortRun);

    const auto success = addTheRemainingEntries(123);
    assert(success);
    assert(state.size() == numEntries);
    // data = state;
}

//  GcUtils::onlyDifferByOneBit(unsigned totalCount, unsigned numBits, const std::vector<uint16_t>& data1, unsigned candidate)

inline bool GcGenerator::isThisStateValid() const {
    //  SQINFO("can this follow (%d) size =%d", candidate, state.size());
#ifdef _LOG
    printf("isThisStateValid called on:\n");
    GcUtils::dumpBinary(nBits, state);
#endif
    // if we are the first guess, we are fine
    if (state.size() < 2) {
        return true;
    }

    const auto differByOne = GcUtils::onlyDifferByOneBit(numEntries, nBits, state);
    if (!differByOne) {
#ifdef _LOG
        printf("not valid, differ by one failed\n");
#endif
        return false;
    }

    const auto candidate = state.back();
    for (unsigned i = 0; i < state.size() - 1; ++i) {
        const auto x = state[i];
        if (x == candidate) {
#ifdef _LOG
            printf("not valid, two entries same: %x, %x\n", x, candidate);
#endif
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

inline bool GcGenerator::isFinalStateOk() {
    if (wantBalanced) {
        const auto dist = GcUtils::getTranstionDataWithWrap(nBits, state);  // this won't look for round abouts!
        const bool isBalanced = dist.first == dist.second;
        if (!isBalanced) {
#if defined(_LOG) || 0
            printf("not balanced, dist = %d,%d\n", dist.first, dist.second);
#endif
            return false;
        }
    }
    // if they are all made, we are good
    if (minShortRun >= 0) {
        const auto shortRun = GcUtils::getShortestRun(nBits, state);
        if (shortRun < minShortRun) {
            if (shortRun < shortestSeen) {
                printf("shortest run = %d, min= %d\n", shortRun, minShortRun);
                shortestSeen = shortRun;
            }
#if defined(_LOG) || 0
            printf("shortest run = %d, min= %d\n", shortRun, minShortRun);
#endif
            return false;
        }
        assert(shortRun >= minShortRun);
    }

    return true;
}

inline bool GcGenerator::addTheRemainingEntries(int marker) {
#ifdef _LOG
    printf("addAnotherENtry called size=%zd\n", state.size());
#endif


    if (state.size() == numEntries) {
        return isFinalStateOk();
    }

    // loop, searching for something to try.
    for (unsigned i = 0; i < nBits; ++i) {
        const auto candidate = getCandidate(i);
#ifdef _LOG
        printf("calling get Candidate from loop i=%d level=%zd cand=%x\n", i, state.size(), candidate);
#endif
        state.push_back(candidate);
        if (isThisStateValid()) {
#ifdef _LOG
            printf("state valid, size=%zd\n", state.size());
#endif
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
#ifdef _LOG
            printf("state bad, size=%zd\n", state.size());
#endif
            state.pop_back();
        }
    }

    // if we got here, this whole stage can't complete.
#ifdef _LOG
    if (state.empty()) {
        printf("giving up on level %zd marker = %d", state.size(), marker);
    } else {
        printf("giving up on level %zd marker = %d top=%x", state.size(), marker, state.back());
    }
#endif
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
