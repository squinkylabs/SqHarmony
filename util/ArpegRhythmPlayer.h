#pragma once

#include "ArpegPlayer.h"

class ArpegRhythmPlayer {
public:
    ArpegRhythmPlayer(ArpegPlayer* underlying) : player(underlying) {}
    std::pair<float, float> clock();  // I think this is legacy? (but still used in some tests that need to port);
    /**
     * @brief clock the player.
     *
     * @return std::tuple<bool, float, float> the first is true if there is something to play, false if no.
     *          If something to play, the values will be in the 2nd and 3rd of the tuple.
     */
    std::tuple<bool, float, float> clock2();
    /**
     * once called, will cause a re-shuffle to happen at the end of current playback,
     * assuming in shuffle mode.
     */
    void armReShuffle() {
        player->armReShuffle();
    }
    void reset();
    void setLength(int len) { length = len; }

private:
    ArpegPlayer* const player;

    // zero means no limit
    int length = 0;
    int counter = 0;
};

inline void ArpegRhythmPlayer::reset() {
    player->reset();
    counter = 0;
}

inline std::pair<float, float> ArpegRhythmPlayer::clock() {
    std::pair<float, float> ret = player->clock();
    if (length && (++counter >= length)) {
        player->reset();
        counter = 0;
    }
    return ret;
}

inline std::tuple<bool, float, float> ArpegRhythmPlayer::clock2() {
    std::tuple<bool, float, float> ret = player->clock2();
    if (length && (++counter >= length)) {
        player->reset();
        counter = 0;
    }
    return ret;
}