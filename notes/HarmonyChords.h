#pragma once

#include <memory>

class Chord4;
class Options;
class Chord4Manager;

using Chord4Ptr = std::shared_ptr<Chord4>;

class HarmonyChords {
public:
    /**
     * @brief
     *
     * @param options
     * @param manager
     * @param prev
     * @param root 1 = root, 2 = 2nd
     * @return Chord4*. could be null.
     *
     * caller does not "own" the chord returned, it is owned my manager
     */
    static const Chord4* findChord(
        bool show,
        const Options& options,
        const Chord4Manager& manager,
        const Chord4& prev,
        int root);

    static const Chord4* findChord(
        bool show,
        const Options& options,
        const Chord4Manager& manager,
        int root);

    static const Chord4* findChord(
        bool show,
        const Options& options,
        const Chord4Manager& manager,
        const Chord4& prevPrev,
        const Chord4& prev,
        int root);

    static int progressionPenalty(const Options& options,
                                  int bestSoFar,
                                  const Chord4* prevProv,
                                  const Chord4* prev,
                                  const Chord4* current,
                                  bool show);

private:
    static const Chord4* find(
        bool show,
        const Options& options,
        const Chord4Manager& manager,
        const Chord4* prevProv,
        const Chord4* prev,
        int root);
};
