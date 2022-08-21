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
    bool canThisFollow(NumType candidate) const;
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

    const auto success = addTheRemainingEntries(123);
    assert(success);
    assert(state.size() == numEntries);
    // data = state;
}

//  GcUtils::onlyDifferByOneBit(unsigned totalCount, unsigned numBits, const std::vector<uint16_t>& data1, unsigned candidate)

inline bool GcGenerator::canThisFollow(NumType candidate) const {
    //  SQINFO("can this follow (%d) size =%d", candidate, state.size());

    // if we are the first guess, we are fine
    if (state.empty()) {
        return true;
    }

    assert(false);  // get rid of candidate stuff
  //  const auto differByOne = GcUtils::onlyDifferByOneBit(numEntries, nBits, state, candidate);
 //   if (!differByOne) {
 //       return false;
//    }

    for (unsigned x : state) {
        if (x == candidate) {
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
    //  SQINFO("addAnotherENtry called size=%d", state.size());
    // first, make next entry

    if (state.size() == numEntries) {
        if (!wantBalanced) {
            return true;
        }
        const auto dist = GcUtils::getTranstionDataWithWrap(nBits, state);  // this won't look for round abouts!
        const bool isBalanced = dist.first == dist.second;
        if (!isBalanced) {
           SQINFO("\nnot balanced, dist = %d,%d", dist.first, dist.second);
           // dump();
            return false;
        }
        // if they are all made, we are good

        return true;
    }

    // otherwise, try to add a good one
    for (unsigned i = 0; i < nBits; ++i) {
        // SQINFO("calling get Candidate from loop i=%d level=%d", i, state.size());
        const auto candidate = getCandidate(i);
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
    }

    // if we got here, this whole stage can't complete.
    if (state.empty()) {
        //SQINFO("giving up on level %d marker = %d", state.size(), marker);
    } else {
        // SQINFO("giving up on level %d marker = %d top=%x", state.size(), marker, state.back());
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
    SQINFO("want bal =%d, var=%d,%d", wantBalanced, x.first, x.second);
    SQINFO("shortest run = %d", GcUtils::getShortestRun(nBits, state));
    SQINFO("num bits=%d stateSize = %d", nBits, state.size());
}
