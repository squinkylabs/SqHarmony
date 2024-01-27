
#include "ArpegPlayer.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <random>
#include <vector>

#include "SqLog.h"

ArpegPlayer::ArpegPlayer(NoteBuffer* nb) : noteBuffer(nb) {
    nb->onChange([this](const NoteBuffer* cbnb) {
        this->dataChanged = true;
    });
    SQDEBUG("ctor of AP, empty=%d", this->empty());
}

void ArpegPlayer::setMode(Mode m) {
    if (m == mode) {
        return;
    }
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
    }

    if (playbackSize < 1) {
        return std::make_tuple(false, 0.f, 0.f);
    }
    // 3/19 -1 happens now. seems valid
    assert(playbackIndex >= 0);
    const std::tuple<bool, float, float> ret = std::make_tuple(true, playbackBuffer[playbackIndex].first, playbackBuffer[playbackIndex].second);

    ++playbackIndex;
    if (playbackIndex >= playbackSize) {
        playbackIndex = 0;
        if (reFillOnIndexArmed) {
            reFillOnIndexArmed = false;
            onIndexWrapAround();
        }
    }

    return ret;
}

void ArpegPlayer::refillPlayback() {
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

    // not sure I trust this, so do it twice
    for (int i = 0; i < 2; ++i) {
        std::shuffle(playbackBuffer, playbackBuffer + numNotes, randomGenerator);
    }

    playbackSize = numNotes;
}

void ArpegPlayer::refillPlaybackUP() {
    SQDEBUG("ArpegPlayer::refillPlaybackUP()");
    copyAndSort();
    for (int i = 0; i < noteBuffer->size(); ++i) {
        playbackBuffer[i] = sortBuffer[i];
    }
    // todo: don't do full reset on playback change
    playbackSize = noteBuffer->size();
    SQDEBUG("ArpegPlayer::refillPlaybackUP() leave with size=%d", playbackSize);
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
    }
    // TODO: test with one entry
    const int downwardEntries = noteBuffer->size() - 2;

    if (downwardEntries <= 0) {
        playbackSize = noteBuffer->size();
        return;
    }
    for (int i = 0; i < downwardEntries; ++i) {
        const int dest = i + noteBuffer->size();
        const int src = noteBuffer->size() - (i + 2);
        playbackBuffer[dest] = sortBuffer[src];
    }

    playbackSize = -2 + 2 * noteBuffer->size();
}

void ArpegPlayer::refillPlaybackUP_DOWN_DBL() {
    copyAndSort();

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

    const int downwardEntries = noteBuffer->size();

    if (downwardEntries <= 0) {
        return;
    }

    for (int i = 1; i < downwardEntries; ++i) {
        const int dest = i + noteBuffer->size() + 0;
        const int src = noteBuffer->size() - (i + 1);
        playbackBuffer[dest] = sortBuffer[src];
    }

    playbackSize = 2 * noteBuffer->size();
}

void ArpegPlayer::refillPlaybackDOWN_UP_DBL() {
    copyAndSort();
    const int siz = noteBuffer->size();

    // first the "up" part
    for (int i = 0; i < (siz); ++i) {
        const int src = i;
        const int dest = -1 + noteBuffer->size() - i;
        playbackBuffer[dest] = sortBuffer[src];
    }

    const int src = 0;  // siz - 1;
    const int dest = siz;
    playbackBuffer[dest] = sortBuffer[src];

    const int downwardEntries = noteBuffer->size();

    if (downwardEntries <= 0) {
        playbackSize = 2 * noteBuffer->size();
        return;
    }

    for (int i = 1; i < downwardEntries; ++i) {
        const int src = 0 + i;
        const int dest = i + noteBuffer->size();
        playbackBuffer[dest] = sortBuffer[src];
    }

    playbackSize = 2 * noteBuffer->size();
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

    int lowIndex = 0;
    int highIndex = siz - 1;

    int destIndex = 0;
    for (int i = 0; i < siz / 2; ++i) {
        int src = highIndex;
        int dest = destIndex++;
        playbackBuffer[dest] = sortBuffer[src];

        src = lowIndex;
        dest = destIndex++;
        playbackBuffer[dest] = sortBuffer[src];

        --highIndex;
        ++lowIndex;
    }
    if (isOdd) {
        playbackBuffer[destIndex] = sortBuffer[medianIndex];
    }
}

void ArpegPlayer::refillPlaybackORDER_PLAYED() {
    const int siz = noteBuffer->size();
    playbackSize = siz;
    for (int i = 0; i < siz; ++i) {
        auto nbPtr = (i + noteBuffer->begin());
        playbackBuffer[i] = std::make_pair(nbPtr->cv1, nbPtr->cv2);
    }
}

void ArpegPlayer::refillPlaybackREPEAT_TOP() {
    copyAndSort();
    const int siz = noteBuffer->size();
    const int end = siz - 1;
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
        playbackBuffer[dest] = sortBuffer[end];

        int src = i;
        ++dest;
        playbackBuffer[dest] = sortBuffer[src];
        assert(src < siz);
    }
    playbackSize = dest + 1;
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