#pragma once

#include "AudioMath.h"
#include "NoteBuffer.h"

#include <algorithm>
#include <random>
#include <string>
#include <vector>

class ArpegPlayer {
public:
    enum class Mode {
        UP,
        DOWN,
        UPDOWN,  // normal, playes extremes once
        DOWNUP,
        UP_DOWN_DBL,  // plays extremes twice
        DOWN_UP_DBL,
        INSIDE_OUT,
        OUTSIDE_IN,
        ORDER_PLAYED,
        REPEAT_BOTTOM,
        REPEAT_TOP,
        SHUFFLE
    };
    ArpegPlayer(NoteBuffer* nb);

    /** clock it and return the next note.
     * At end of index it may reshuffle, but only if reFillOnIndex is true
     */
    std::pair<float, float> clock();

     /** 
      * clock it and return the next note.
      * At end of index it may reshuffle, but only if reFillOnIndex is true
      * tuple 0: if false ignore
      * tuple 1: cv 1
      * tuple 2: cv 2
      */
    std::tuple<bool, float, float> clock2();

    /**
     * once called, will cause a re-shuffle to happen at the end of current playback,
     * assuming in shuffle mode.
     */
    void armReShuffle() {
        reFillOnIndexArmed = true;
    }
    bool empty() const;

    void setMode(Mode m);
    static std::vector<std::string> modes() {
        return {"up", "down", "up+down", "down+up", "up then down", "down then up", "inside-out", "outside-in",
                "order played", "repeat low", "repeat high", "shuffle"};
    }

    static std::vector<std::string> shortModes() {
        return {"up", "down", "updown", "downup", "up&down", "down&up", "inside-out", "outside-in",
                "in order", "rep-low", "rep-high", "shuffle"};
    }

    void reset();

private:
  
    bool dataChanged = true;
    NoteBuffer* const noteBuffer = nullptr;
    Mode mode{Mode::UP};

    // use std C here
   // AudioMath::RandomUniformFunc random = {AudioMath::random()};
    std::mt19937 randomGenerator{1234567891};

    std::pair<float, float> playbackBuffer[4 + 2 * (1 + NoteBuffer::maxCapacity)];
    std::pair<float, float>  sortBuffer[NoteBuffer::maxCapacity];
    int playbackIndex = -1;
    int playbackSize = 0;
    bool reFillOnIndexArmed = false;

    void onIndexWrapAround();

    void udpatePlayback();
    void checkUpdatePlayback();
    void copyAndSort();
    void refillPlayback();
    void refillPlaybackUP();
    void refillPlaybackDOWN();
    void refillPlaybackUPDOWN();
    void refillPlaybackDOWNUP();
    void refillPlaybackDOWN_UP_DBL();
    void refillPlaybackUP_DOWN_DBL();
    void refillPlaybackINSIDE_OUT();
    void refillPlaybackOUTSIDE_IN();
    void refillPlaybackORDER_PLAYED();
    void refillPlaybackREPEAT_BOTTOM();
    void refillPlaybackREPEAT_TOP();
    void refillPlaybackSHUFFLE();
};
