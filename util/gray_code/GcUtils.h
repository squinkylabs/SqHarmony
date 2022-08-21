
#pragma once
#include <vector>
#include "SqLog.h"

class GcUtils {
public:
    static unsigned numBitsDifferent(unsigned numBits, unsigned a, unsigned b);

    /**
     * @brief tests that the element at the back of the array only has one bit difference
     * from the previous one. if data is full, then also tests for wrap around
     */
    static bool onlyDifferByOneBit(unsigned totalCount, unsigned numBits, const std::vector<uint16_t>& data);

//    static bool onlyDifferByOneBit(unsigned totalCount, unsigned numBits, const std::vector<uint16_t>& data1, unsigned candidate);

    /**
     * Analyze a vector of data + a word for high and low counts of bit transitions.
     * count transition from data1[last] -> data2, and data2 -> data1[0]
     */
    static std::pair<unsigned, unsigned> getTranstionData(unsigned numBits, std::vector<uint16_t>& data1, uint16_t data2);

    /**
     * Analyze a vector of data for high and low counts of bit transitions.
     * do not count roll over from end -> start
     */
    static std::pair<unsigned, unsigned> getTranstionData(unsigned numBits, std::vector<uint16_t>& data);

    /**
     * Analyze a vector of data for high and low counts of bit transitions.
     * DOES count roll over from end -> start
     */
    static std::pair<unsigned, unsigned> getTranstionDataWithWrap(unsigned numBits, std::vector<uint16_t>& data);

    static void dumpBinary(unsigned nBits, unsigned data);

    /**
     * @brief Get the Shortest Run of bits.
     * Will wrap around the end looking.
     * If all values are the same, it implies an infinitely long cycle. Of course this
     * is impossible in a grey code, but in the case of all the same we just return the array length + 1
     *
     * @param numBits - is the number of bits in the data
     * @param data - is the array to search for runs
     * @return length of shortest run, or -1 if can't evaluate
     */
    static int getShortestRun(unsigned numBits, const std::vector<uint16_t>& data);

    template <typename T>
    static T getWithRollover(const std::vector<T>& v, unsigned index);
};

inline void GcUtils::dumpBinary(unsigned nBits, unsigned data) {
    for (int i = nBits - 1; i >= 0; --i) {
        const auto mask = 1 << i;
        const bool bit = data & mask;
        printf(" %d", bit);
    }
}

inline bool GcUtils::onlyDifferByOneBit(unsigned totalCount, unsigned numBits, const std::vector<uint16_t>& data) {
    if (data.size() < 2) {
        return false;
    }

    // look at the last two in the array
    const auto num = numBitsDifferent(numBits, data.back(), data[data.size()-2]);
    if (num != 1) {
        return false;
    }

    // if array is full, need to test wrap around
    if (data.size() >= (totalCount - 1)) {
        const auto num = numBitsDifferent(numBits, data.front(), data.back());
        if (num != 1) {
            return false;
        }
    }

    return true;
}

#if 0
inline bool GcUtils::onlyDifferByOneBit(unsigned totalCount, unsigned numBits, const std::vector<uint16_t>& data1, unsigned candidate) {
    if (data1.empty()) {
        return false;
    }

    const auto num = numBitsDifferent(numBits, data1.back(), candidate);
    if (num != 1) {
        return false;
    }

    // if candidate could be last element we need to look at wrap around
    if (data1.size() >= (totalCount - 1)) {
        const auto num = numBitsDifferent(numBits, data1.front(), candidate);
        if (num != 1) {
            return false;
        }
    }

    return true;  // ignoring total count
}
#endif

inline unsigned GcUtils::numBitsDifferent(unsigned numBits, unsigned a, unsigned b) {
    unsigned count = 0;
    for (unsigned i = 0; i < numBits; ++i) {
        unsigned mask = 1 << i;
        if ((a & mask) != (b & mask)) {
            ++count;
        }
    }
    return count;
}

inline std::pair<unsigned, unsigned> GcUtils::getTranstionData(unsigned numBits, std::vector<uint16_t>& data) {
    if (data.size() < 2) {
        return std::pair<unsigned, unsigned>(0, 0);
    }
    unsigned highest = 0;
    unsigned lowest = 10000;
    for (unsigned bit = 0; bit < numBits; ++bit) {
        const auto mask = 1 << bit;
        unsigned transitions = 0;
        assert(data.size() > 0);
        auto last = data[0] & mask;
        for (unsigned index = 1; index < data.size(); ++index) {
            const auto x = data[index] & mask;
            if (x != last) {
                transitions++;
            }
            last = x;
        }
        highest = std::max(highest, transitions);
        lowest = std::min(lowest, transitions);
    }

    return std::pair<unsigned, unsigned>(lowest, highest);
}

inline std::pair<unsigned, unsigned> GcUtils::getTranstionData(unsigned numBits, std::vector<uint16_t>& data1, uint16_t data2) {
    if (data1.empty()) {
        return std::pair<unsigned, unsigned>(0, 0);
    }
    data1.push_back(data2);
    data1.push_back(data1.front());
    const auto ret = getTranstionData(numBits, data1);
    data1.pop_back();
    data1.pop_back();
    return ret;
}

inline std::pair<unsigned, unsigned> GcUtils::getTranstionDataWithWrap(unsigned numBits, std::vector<uint16_t>& data) {
    if (data.empty()) {
        return std::pair<unsigned, unsigned>(0, 0);
    }
    data.push_back(data.front());
    const auto ret = getTranstionData(numBits, data);
    data.pop_back();
    return ret;
}

template <typename T>
inline T GcUtils::getWithRollover(const std::vector<T>& v, unsigned index) {
    const auto i = index % v.size();
    return v[i];
}

inline int GcUtils::getShortestRun(unsigned numBits, const std::vector<uint16_t>& data) {
    if (data.empty()) {
        return -1;
    }
    SQINFO("enter");

    const auto indexMax = data.size() * 2;

    // arbitrarily this is how we define the runs if no change.
    // since it never happens, who cares?
    int shortestRun = int(data.size());
    for (unsigned bit = 0; bit < numBits; ++bit) {
        const auto mask = 1 << bit;
        // SQINFO("-- top of loop, bit %d mask = %x", bit, mask);
        int runCounter = 0;
        unsigned lastX = -1;
        bool haveSeenChange = false;

        for (auto i = 0; i < indexMax; ++i) {
            const auto d = getWithRollover(data, i);
            const auto x = d & mask;
            // SQINFO("top of inner loop i=%d, d=%x x=%x firstTime=%d", i, d, x, firstTime);

            // ignore the first data point - no past to compare it to
            if (i != 0) {
                // is there a change in the bit
                if (x != lastX) {
                    if (haveSeenChange) {
                        shortestRun = std::min(shortestRun, runCounter);
                        // SQINFO("found change in bit pattern, captured short=%d i=%d", shortestRun, i);
                      
                        runCounter = 1;
                    }
                    else {
                        runCounter = 0;
                    }
                    haveSeenChange = true;
                }
                ++runCounter;
            }
            lastX = x;
          
        }
    }
    SQINFO("exit");
    return shortestRun;
}