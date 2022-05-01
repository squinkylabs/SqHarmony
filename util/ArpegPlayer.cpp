
#include "ArpegPlayer.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <random>
#include <vector>

#include "SqLog.h"

ArpegPlayer::ArpegPlayer(NoteBuffer* nb) : noteBuffer(nb) {
    // printf("**** ctor of ArpegPlayer\n");
    nb->onChange([this](const NoteBuffer* cbnb) {
        // printf("* ArpegPlayer callback onChange, set dataChange true\n");
        this->dataChanged = true;
    });
    SQDEBUG("ctor of AP, empty=%d", this->empty());
}

void ArpegPlayer::setMode(Mode m) {
    if (m == mode) {
        return;
    }
    // printf("set mode, change mode was %d is %d\n", int(mode), int(m));
    mode = m;
    dataChanged = true;
}

void ArpegPlayer::reset() {
    playbackIndex = -1;  // force start at start
    dataChanged = true;
}

bool ArpegPlayer::empty() const {
    return playbackSize < 1;
}

std::pair<float, float> ArpegPlayer::clock() {
    const auto x = clock2();
    return std::make_pair(std::get<1>(x), std::get<2>(x));
}

std::tuple<bool, float, float> ArpegPlayer::clock2() {
    // on a data change, let's try to find the next note to play
    if (dataChanged) {
        // remember where we were
        const float pitchWouldBe = (playbackIndex >= 0) ? playbackBuffer[playbackIndex].first : -100;
        const int playbackIndexWouldBe = playbackIndex;

        SQDEBUG("arp::clock sees data change would be %d, will refill", playbackIndexWouldBe);
        // incorporate the new data
        refillPlayback();

        assert(playbackIndexWouldBe == playbackIndex);
        //  assert(playbackIndexWouldBe >= 0);

        // if we are just starting up, no previous info we care about
        if (playbackIndexWouldBe < 0) {
            playbackIndex = 0;  // 3/19 used to be zero
            SQDEBUG("arp::clock sees data change set pb to -1 so will inc to zero");
        }

        bool foundSettings = false;

        // 1) if the same note we expected to play is still there, use that
        if ((playbackIndexWouldBe < playbackSize) && (pitchWouldBe == playbackBuffer[playbackIndexWouldBe].first)) {
            // printf("after reset, use same\n");
            foundSettings = true;
        }
        // 2) if we can find the same note elsewhere, use it
        if (!foundSettings) {
            for (int i = 0; !foundSettings && (i < playbackSize); ++i) {
                if (playbackBuffer[i].first == pitchWouldBe) {
                    // printf("found old pitch at %d\n", i);
                    foundSettings = true;
                    playbackIndex = i;
                }
            }
        }

        // 3) if the step we were going to play is still valid, use that
        if (!foundSettings) {
            if (playbackIndexWouldBe < playbackSize) {
                foundSettings = true;
            }
        }

        // 4) if zero is valid index, use that
        if (!foundSettings) {
            if (playbackSize > 0) {
                playbackIndex = 0;
                foundSettings = true;
            }
        }

        // 5) if no notes at all, get ready to leave
        if (!foundSettings) {
            if (playbackSize == 0) {
                // printf("set playback index to -1 at 90\n");
                playbackIndex = -1;
                foundSettings = true;
            }
        }
        assert(foundSettings);
        dataChanged = false;
        // SQINFO("end of data change in clock, index=%d", playbackIndex);
    }

    if (playbackSize < 1) {
        // SQINFO("nothing to play, will ret zero");
        return std::make_tuple(false, 0.f, 0.f);
    }
    // 3/19 -1 happens now. seems valid
    assert(playbackIndex >= 0);
    const std::tuple<bool, float, float> ret = std::make_tuple(true, playbackBuffer[playbackIndex].first, playbackBuffer[playbackIndex].second);

    // SQINFO("ArpegPlayer::clock will ret %d,%f,%f from index %d", std::get<0>(ret), std::get<1>(ret), std::get<2>(ret), playbackIndex);

    ++playbackIndex;
    //  assert(playbackIndex >= 0);
    // SQINFO("ArpegPlayer::clock at end index=%d size=%d", playbackIndex, playbackSize);
    if (playbackIndex >= playbackSize) {
        // SQINFO("ArpegPlayer::clock post inc sees wrap %d, %d. refill flag=%d", playbackIndex, playbackSize, reFillOnIndexArmed);
        playbackIndex = 0;
        if (reFillOnIndexArmed) {
            // SQINFO("XX wrapped on arm");
            reFillOnIndexArmed = false;
            onIndexWrapAround();
        }
    }

    return ret;
}

void ArpegPlayer::refillPlayback() {
    // SQINFO("ArpegPlayer::refillPlayback nb has %d", noteBuffer->size());
    switch (mode) {
        case Mode::UP:
            refillPlaybackUP();
            break;
        case Mode::DOWN:
            refillPlaybackDOWN();
            break;
        case Mode::UPDOWN:
            refillPlaybackUPDOWN();
            break;
        case Mode::DOWNUP:
            refillPlaybackDOWNUP();
            break;
        case Mode::DOWN_UP_DBL:
            refillPlaybackDOWN_UP_DBL();
            break;
        case Mode::UP_DOWN_DBL:
            refillPlaybackUP_DOWN_DBL();
            break;
        case Mode::INSIDE_OUT:
            refillPlaybackINSIDE_OUT();
            break;
        case Mode::OUTSIDE_IN:
            refillPlaybackOUTSIDE_IN();
            break;
        case Mode::ORDER_PLAYED:
            refillPlaybackORDER_PLAYED();
            break;
        case Mode::REPEAT_BOTTOM:
            refillPlaybackREPEAT_BOTTOM();
            break;
        case Mode::REPEAT_TOP:
            refillPlaybackREPEAT_TOP();
            break;
        case Mode::SHUFFLE:
            refillPlaybackSHUFFLE();
            break;
        default:
            assert(false);
    }
}

void ArpegPlayer::onIndexWrapAround() {
    // SQINFO("on index wrap around");
    //  most modes don't care
    if (mode == Mode::SHUFFLE) {
        assert(playbackSize == noteBuffer->size());
        refillPlaybackSHUFFLE();
    }
}

void ArpegPlayer::copyAndSort() {
    auto input = noteBuffer->begin();
    for (int i = 0; i < noteBuffer->size(); ++i) {
        sortBuffer[i] = std::make_pair(input->cv1, input->cv2);
        ++input;
    }
    // ascending sort
    std::sort(sortBuffer, sortBuffer + noteBuffer->size());
}

void ArpegPlayer::refillPlaybackSHUFFLE() {
    assert(this);
    assert(noteBuffer);
    // SQINFO("this = %p, noteBuffer = %p nb->size = %d", this, noteBuffer, noteBuffer->size());

    const int numNotes = noteBuffer->size();
    // loop, filling up sortBuffer[copyIndex] each time
    for (int copyIndex = 0; copyIndex < numNotes; ++copyIndex) {
        playbackBuffer[copyIndex] = std::make_pair(noteBuffer->at(copyIndex).cv1, noteBuffer->at(copyIndex).cv2);
    };

#if 0
    for (int copyIndex = 0; copyIndex < numNotes; ++copyIndex) {
        SQINFO("before shuffle %f,%f", playbackBuffer[copyIndex].first, playbackBuffer[copyIndex].second);
    };
#endif

    // not sure I trust this, so do it twice
    for (int i = 0; i < 2; ++i) {
        std::shuffle(playbackBuffer, playbackBuffer + numNotes, randomGenerator);
    }

#if 0
    for (int copyIndex = 0; copyIndex < numNotes; ++copyIndex) {
        SQINFO("after shuffle %f,%f", playbackBuffer[copyIndex].first, playbackBuffer[copyIndex].second);
    };
#endif

    playbackSize = numNotes;
}

void ArpegPlayer::refillPlaybackUP() {
    SQDEBUG("ArpegPlayer::refillPlaybackUP()");
    copyAndSort();
    for (int i = 0; i < noteBuffer->size(); ++i) {
        playbackBuffer[i] = sortBuffer[i];
    }
    // todo: don't do full reset on playback change
    // playbackIndex = 0;
    playbackSize = noteBuffer->size();
    SQDEBUG("ArpegPlayer::refillPlaybackUP() leave with size=%d", playbackSize);
    // printf("refillPlaybackUP set index to %d size=%d\n", playbackIndex, playbackSize);
}

void ArpegPlayer::refillPlaybackDOWN() {
    copyAndSort();
    playbackSize = noteBuffer->size();
    for (int i = 0; i < noteBuffer->size(); ++i) {
        playbackBuffer[-1 + playbackSize - i] = sortBuffer[i];
    }
    // todo: don't do full reset on playback change
    //  playbackIndex = 0;
}

void ArpegPlayer::refillPlaybackUPDOWN() {
    copyAndSort();

    // printf("update, there are %d\n", noteBuffer->size());
    //  first the "up" part
    for (int i = 0; i < noteBuffer->size(); ++i) {
        playbackBuffer[i] = sortBuffer[i];
        // printf("copy %d to %d (%f)\n", i, i, sortBuffer[i]);
    }
    // TODO: test with one entry
    const int downwardEntries = noteBuffer->size() - 2;
    // printf("there are %d downward entries\n", downwardEntries);

    if (downwardEntries <= 0) {
        playbackSize = noteBuffer->size();
        // playbackIndex = 0;
        // printf("leaving on no downward\n");
        return;
    }
    for (int i = 0; i < downwardEntries; ++i) {
        const int dest = i + noteBuffer->size();
        const int src = noteBuffer->size() - (i + 2);
        playbackBuffer[dest] = sortBuffer[src];
        // printf("copy %d to %d (%f) \n", src, dest, sortBuffer[src]);
    }

    playbackSize = -2 + 2 * noteBuffer->size();
    // printf("leaving with playback size = %d\n", playbackSize);
}

void ArpegPlayer::refillPlaybackUP_DOWN_DBL() {
    copyAndSort();

    // printf("update, UP_DOWN_DB there are %d\n", noteBuffer->size());
    const int siz = noteBuffer->size();

    // first the "up" part
    for (int i = 0; i < siz; ++i) {
        const int src = i;
        const int dest = i;
        playbackBuffer[dest] = sortBuffer[src];
        // printf("copy %d to %d (%f)\n", src, dest, sortBuffer[src]);
    }

    const int src = siz - 1;
    const int dest = siz;
    playbackBuffer[dest] = sortBuffer[src];
    // printf("copy extra end %d to %d (%f)\n", src, dest, sortBuffer[src]);

    const int downwardEntries = noteBuffer->size();
    // printf("there are %d downward entries\n", downwardEntries);

    if (downwardEntries <= 0) {
        playbackSize = 2 * noteBuffer->size();
        // printf("leaving on no downward, pb size=%d\n", playbackSize);
        return;
    }

    for (int i = 1; i < downwardEntries; ++i) {
        const int dest = i + noteBuffer->size() + 0;
        const int src = noteBuffer->size() - (i + 1);
        playbackBuffer[dest] = sortBuffer[src];
        // ("dn copy %d to %d (%f) \n", src, dest, sortBuffer[src]);
    }

    playbackSize = 2 * noteBuffer->size();
    // printf("leaving with playback size = %d\n", playbackSize);
}

void ArpegPlayer::refillPlaybackDOWN_UP_DBL() {
    copyAndSort();
    // printf("update, there are %d\n", noteBuffer->size());
    const int siz = noteBuffer->size();

    // first the "up" part
    for (int i = 0; i < (siz); ++i) {
        const int src = i;
        const int dest = -1 + noteBuffer->size() - i;
        playbackBuffer[dest] = sortBuffer[src];
        // printf("copy %d to %d (%f)\n", src, dest, sortBuffer[src]);
    }

    const int src = 0;  // siz - 1;
    const int dest = siz;
    playbackBuffer[dest] = sortBuffer[src];
    // printf("copy extra end %d to %d (%f)\n", src, dest, sortBuffer[src]);

    const int downwardEntries = noteBuffer->size();
    // printf("there are %d downward entries\n", downwardEntries);

    if (downwardEntries <= 0) {
        playbackSize = 2 * noteBuffer->size();
        //   playbackIndex = 0;
        // printf("leaving on no downward, pb size=%d\n", playbackSize);
        return;
    }

    for (int i = 1; i < downwardEntries; ++i) {
        const int src = 0 + i;
        const int dest = i + noteBuffer->size();
        playbackBuffer[dest] = sortBuffer[src];
        // printf("copy %d to %d (%f) \n", src, dest, sortBuffer[src]);
    }

    playbackSize = 2 * noteBuffer->size();
    // printf("leaving with playback size = %d\n", playbackSize);
    for (int i = 0; i < playbackSize; ++i) {
        // printf("* final output[% d] = % f\n", i, playbackBuffer[i]);
    }
}

void ArpegPlayer::refillPlaybackDOWNUP() {
    copyAndSort();
    for (int i = 0; i < noteBuffer->size(); ++i) {
        const int src = i;
        const int dest = -1 + noteBuffer->size() - i;
        assert(dest >= 0);
        playbackBuffer[dest] = sortBuffer[src];
    }
    const int upwardEntries = noteBuffer->size() - 2;
    if (upwardEntries <= 0) {
        playbackSize = noteBuffer->size();
        return;
    }

    for (int i = 0; i < upwardEntries; ++i) {
        // this src can't be right
        const int src = 1 + i;
        const int dest = i + noteBuffer->size();
        playbackBuffer[dest] = sortBuffer[src];
    }

    playbackSize = -2 + 2 * noteBuffer->size();
}

void ArpegPlayer::refillPlaybackINSIDE_OUT() {
    copyAndSort();
    const int siz = noteBuffer->size();
    playbackSize = siz;
    if (siz < 1) {
        return;
    }

    assert(siz > 0);
    const bool isOdd = siz & 1;

    const int medianIndex = (siz / 2);
    assert(medianIndex < siz);
    // printf("refillPlaybackINSIDE_OUT, there are %d med=%d\n", noteBuffer->size(), medianIndex);

    int lowIndex = medianIndex;
    int highIndex = medianIndex;
    playbackBuffer[0] = sortBuffer[medianIndex];
    // printf("copied first entry(0) from %d val %f\n", medianIndex, playbackBuffer[0]);

    int destIndex = 1;
    for (bool done = false; !done;) {
        done = true;
        ++highIndex;
        --lowIndex;
        if (isOdd) {
            if (highIndex < siz) {
                done = false;
                const int src = highIndex;
                const int dest = destIndex++;
                playbackBuffer[dest] = sortBuffer[src];
                // printf("copied high src=%d dest=%d val=%f\n", src, dest, playbackBuffer[dest]);
            }
        }
        if (lowIndex >= 0) {
            done = false;
            const int src = lowIndex;
            const int dest = destIndex++;
            playbackBuffer[dest] = sortBuffer[src];
            // printf("copied low src=%d dest=%d val=%f\n", src, dest, playbackBuffer[dest]);
        }
        if (!isOdd) {
            if (highIndex < siz) {
                done = false;
                const int src = highIndex;
                const int dest = destIndex++;
                playbackBuffer[dest] = sortBuffer[src];
                // printf("copied high src=%d dest=%d val=%f\n", src, dest, playbackBuffer[dest]);
            }
        }
    }
}

void ArpegPlayer::refillPlaybackOUTSIDE_IN() {
    copyAndSort();
    const int siz = noteBuffer->size();
    playbackSize = siz;

    if (siz < 1) {
        return;
    }

    assert(siz > 0);
    const bool isOdd = siz & 1;

    // const int medianIndex = -1 + (siz + 1) / 2;

    const int medianIndex = (siz / 2);
    assert(medianIndex < siz);
    // printf("refillPlaybackOUTSIDE_IN, there are %d med=%d isOdd=%d\n", noteBuffer->size(), medianIndex, isOdd);

    int lowIndex = 0;
    int highIndex = siz - 1;
    // playbackBuffer[0] = sortBuffer[medianIndex];
    // printf("copied first entry(0) from %d val %f\n", medianIndex, playbackBuffer[0]);

    int destIndex = 0;
    // for (bool done = false; !done;) {
    for (int i = 0; i < siz / 2; ++i) {
        // printf("in out loop, i=%d\n", i);

        int src = highIndex;
        int dest = destIndex++;
        playbackBuffer[dest] = sortBuffer[src];
        // printf("copied high src=%d dest=%d val=%f\n", src, dest, playbackBuffer[dest]);

        src = lowIndex;
        dest = destIndex++;
        playbackBuffer[dest] = sortBuffer[src];
        // printf("copied low src=%d dest=%d val=%f\n", src, dest, playbackBuffer[dest]);

        --highIndex;
        ++lowIndex;
    }
    if (isOdd) {
        // printf("need to copy middle?\n");
        playbackBuffer[destIndex] = sortBuffer[medianIndex];
        // printf("copied last dest=%d src= %d val %f\n", destIndex, medianIndex, playbackBuffer[destIndex]);
    }
}

void ArpegPlayer::refillPlaybackORDER_PLAYED() {
    const int siz = noteBuffer->size();
    playbackSize = siz;
    for (int i = 0; i < siz; ++i) {
        auto nbPtr = (i + noteBuffer->begin());
        playbackBuffer[i] = std::make_pair(nbPtr->cv1, nbPtr->cv2);
        // printf("sorted input[%d] = %f\n", i, sortBuffer[i]);
    }
}

void ArpegPlayer::refillPlaybackREPEAT_TOP() {
    copyAndSort();
    const int siz = noteBuffer->size();
    const int end = siz - 1;
    int dest = 0;
    // printf("in rep bottom, siz=%d\n", siz);
    if (siz == 0) {
        playbackSize = 0;
        return;
    }
    if (siz == 1) {
        playbackSize = 1;
        playbackBuffer[0] = sortBuffer[0];
        return;
    }
    for (int i = 0; i < siz - 1; ++i) {
        // printf("in loop, i=%d\n", i);
        dest = i * 2;
        playbackBuffer[dest] = sortBuffer[end];
        // printf("copyed 0 to %d (%f)\n", dest, sortBuffer[end]);

        int src = i;
        ++dest;
        playbackBuffer[dest] = sortBuffer[src];
        // printf("copyed %d to %d (%f)\n", src, dest, sortBuffer[src]);
        assert(src < siz);
    }
    playbackSize = dest + 1;
    // printf("playback size = %d\n", playbackSize);
}

void ArpegPlayer::refillPlaybackREPEAT_BOTTOM() {
    copyAndSort();
    const int siz = noteBuffer->size();
    int dest = 0;
    if (siz == 0) {
        playbackSize = 0;
        return;
    }
    if (siz == 1) {
        playbackSize = 1;
        playbackBuffer[0] = sortBuffer[0];
        return;
    }
    for (int i = 0; i < siz - 1; ++i) {
        dest = i * 2;
        playbackBuffer[dest] = sortBuffer[0];

        int src = 1 + i;
        ++dest;
        playbackBuffer[dest] = sortBuffer[src];
        assert(src < siz);
    }
    playbackSize = dest + 1;
}