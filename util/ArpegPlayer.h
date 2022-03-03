#pragma once

#include "AudioMath.h"
#include "NoteBuffer.h"

#include <algorithm>
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

    /** clock it and get the next note
     */
    std::pair<float, float> clock();

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
    NoteBuffer* const noteBuffer;
    Mode mode{Mode::UP};

    AudioMath::RandomUniformFunc random = {AudioMath::random()};

    std::pair<float, float> playbackBuffer[4 + 2 * (1 + NoteBuffer::maxCapacity)];
    std::pair<float, float>  sortBuffer[NoteBuffer::maxCapacity];
   // bool needUpdate = true;
    int playbackIndex = -1;
    int playbackSize = 0;

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
