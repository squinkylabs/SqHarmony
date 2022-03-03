#pragma once

#include "ArpegPlayer.h"

class ArpegRhythmPlayer {
public:
    ArpegRhythmPlayer(ArpegPlayer* underlying) : player(underlying) {}
    std::pair<float, float> clock();
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

inline  std::pair<float, float> ArpegRhythmPlayer::clock() {
    std::pair<float, float> ret = player->clock();
    if (length && (++counter >= length)) {
        player->reset();
        counter = 0;
    }
    return ret;
}
