
#include "ArpegPlayer.h"

ArpegPlayer::ArpegPlayer(NoteBuffer* nb) : noteBuffer(nb) {
    // printf("**** ctor of ArpegPlayer\n");
    nb->onChange([this](const NoteBuffer* cbnb) {
        // printf("* ArpegPlayer callback onChange, set dataChange true\n");
        this->dataChanged = true;
    });
}

void ArpegPlayer::setMode(Mode m) {
    if (m == mode) {
        return;
    }
    // printf("set mode, change mode was %d is %d\n", int(mode), int(m));
    mode = m;
    dataChanged = true;
}

//int debug = 0;
void ArpegPlayer::reset() {
    // printf("**AP::reset called, set index to -1 debug=%d\n", debug);
    playbackIndex = -1;  // force start at start
    dataChanged = true;
    // resetInfo.indexLastPlayed = -1;

    static int times = 0;
    if (++times > 4) {
        // this is just a test
        // assert(false);
    }
}

float ArpegPlayer::clock() {
    // printf("  enter clock index=%d (will use this one) data changed =%d\n", playbackIndex, dataChanged);

    // on a data change, let's try to find the next note to play
    if (dataChanged) {
        // printf("in clock, detected data change. notes in nb=%d siz=%d\n", noteBuffer->size(), playbackSize);

        // remember where we were
        const float pitchWouldBe = (playbackIndex >= 0) ? playbackBuffer[playbackIndex] : -100;
        const int playbackIndexWouldBe = playbackIndex;

        // incorporate the new data
        refillPlayback();
        assert(playbackIndexWouldBe == playbackIndex);
        //  assert(playbackIndexWouldBe >= 0);

        // if we are just starting up, no previous info we care about
        if (playbackIndexWouldBe < 0) {
            playbackIndex = 0;
        }

        bool foundSettings = false;

        // 1) if the same note we expected to play is still there, use that
        if ((playbackIndexWouldBe < playbackSize) && (pitchWouldBe == playbackBuffer[playbackIndexWouldBe])) {
            // printf("after reset, use same\n");
            foundSettings = true;
        }
        // 2) if we can find the same note elsewhere, use it
        if (!foundSettings) {
            for (int i = 0; !foundSettings && (i < playbackSize); ++i) {
                if (playbackBuffer[i] == pitchWouldBe) {
                    //printf("found old pitch at %d\n", i);
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
                //printf("set playback index to -1 at 90\n");
                playbackIndex = -1;
                foundSettings = true;
            }
        }
        assert(foundSettings);
        dataChanged = false;
    }

    if (playbackSize < 1) {
        return 0;
    }
    assert(playbackIndex >= 0);
    const float ret = playbackBuffer[playbackIndex];
    //  resetInfo.pitchLastPlayed = ret;
    //  resetInfo.indexLastPlayed = playbackIndex;

    ++playbackIndex;
    //printf("** inc playback index to %d size =%d\n", playbackIndex, playbackSize);
    if (playbackIndex >= playbackSize) {
        playbackIndex = 0;
        //("playback index wrapped around\n");
        onIndexWrapAround();
    }
   //printf("  leave clock index=%d data=%f flag %d\n", playbackIndex, ret, dataChanged); fflush(stdout);
    return ret;
}

#if 0
void ArpegPlayer::checkUpdatePlayback() {
    printf("in checkUpdatePlayer, flag=%d\n", resetInfo.dataChanged);
    if (!resetInfo.dataChanged) {
        return;
    }

    assert(resetInfo.indexLastPlayed < 0);

    refillPlayback();
    resetInfo.dataChanged = false;
}
#endif

void ArpegPlayer::refillPlayback() {
    // printf("enter reFill playback, size = %d nb has %d\n", playbackSize, noteBuffer->size());
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
    // printf("leave reFill playback, size = %d\n", playbackSize);
}

void ArpegPlayer::onIndexWrapAround() {
    // most modes don't care
    if (mode == Mode::SHUFFLE) {
        assert(playbackSize == noteBuffer->size());
        refillPlaybackSHUFFLE();
    }
}

void ArpegPlayer::copyAndSort() {
    auto input = noteBuffer->begin();
    for (int i = 0; i < noteBuffer->size(); ++i) {
        sortBuffer[i] = input->cv;
        ++input;
    }
    // ascending sort
    std::sort(sortBuffer, sortBuffer + noteBuffer->size());
}

void ArpegPlayer::refillPlaybackSHUFFLE() {
  //  int copyIndex = 0;
    bool didUseNote[NoteBuffer::maxCapacity] = {false};

    const int numNotes = noteBuffer->size();
    // loop, filling up sortBuffer[copyIndex] each time
    // Note: could use std::shuffle instead
    for (int copyIndex = 0; copyIndex < numNotes; ++copyIndex) {
        for (bool done = false; !done;) {
            int rand = int(std::round(numNotes * random()));
            if (rand >= numNotes) {
                rand = numNotes - 1;
            }
           // printf("in inner loop, copy index = %d rand=%d total %d\n", copyIndex, rand, numNotes);
            fflush(stdout);
            if (!didUseNote[rand]) {
                playbackBuffer[copyIndex] = noteBuffer->at(rand).cv;
                done = true;
                didUseNote[rand] = true;
            }
        }
    }
    playbackSize = numNotes;
}

void ArpegPlayer::refillPlaybackUP() {
    copyAndSort();
    for (int i = 0; i < noteBuffer->size(); ++i) {
        playbackBuffer[i] = sortBuffer[i];
    }
    // todo: don't do full reset on playback change
    // playbackIndex = 0;
    playbackSize = noteBuffer->size();
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

    //printf("update, there are %d\n", noteBuffer->size());
    // first the "up" part
    for (int i = 0; i < noteBuffer->size(); ++i) {
        playbackBuffer[i] = sortBuffer[i];
        //printf("copy %d to %d (%f)\n", i, i, sortBuffer[i]);
    }
    // TODO: test with one entry
    const int downwardEntries = noteBuffer->size() - 2;
    //printf("there are %d downward entries\n", downwardEntries);

    if (downwardEntries <= 0) {
        playbackSize = noteBuffer->size();
        // playbackIndex = 0;
        //printf("leaving on no downward\n");
        return;
    }
    for (int i = 0; i < downwardEntries; ++i) {
        const int dest = i + noteBuffer->size();
        const int src = noteBuffer->size() - (i + 2);
        playbackBuffer[dest] = sortBuffer[src];
        //printf("copy %d to %d (%f) \n", src, dest, sortBuffer[src]);
    }

    playbackSize = -2 + 2 * noteBuffer->size();
    //printf("leaving with playback size = %d\n", playbackSize);
}

void ArpegPlayer::refillPlaybackUP_DOWN_DBL() {
    copyAndSort();

    //printf("update, UP_DOWN_DB there are %d\n", noteBuffer->size());
    const int siz = noteBuffer->size();

    // first the "up" part
    for (int i = 0; i < siz; ++i) {
        const int src = i;
        const int dest = i;
        playbackBuffer[dest] = sortBuffer[src];
        //printf("copy %d to %d (%f)\n", src, dest, sortBuffer[src]);
    }

    const int src = siz - 1;
    const int dest = siz;
    playbackBuffer[dest] = sortBuffer[src];
    //printf("copy extra end %d to %d (%f)\n", src, dest, sortBuffer[src]);

    const int downwardEntries = noteBuffer->size();
    //printf("there are %d downward entries\n", downwardEntries);

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
    //printf("update, there are %d\n", noteBuffer->size());
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
    //printf("copy extra end %d to %d (%f)\n", src, dest, sortBuffer[src]);

    const int downwardEntries = noteBuffer->size();
    //printf("there are %d downward entries\n", downwardEntries);

    if (downwardEntries <= 0) {
        playbackSize = 2 * noteBuffer->size();
        //   playbackIndex = 0;
        //printf("leaving on no downward, pb size=%d\n", playbackSize);
        return;
    }

    for (int i = 1; i < downwardEntries; ++i) {
        // wrong
        // const int dest = i + noteBuffer->size() + 1;
        // const int src = noteBuffer->size() - (i + 1);

        const int src = 0 + i;
        const int dest = i + noteBuffer->size();
        playbackBuffer[dest] = sortBuffer[src];
        //printf("copy %d to %d (%f) \n", src, dest, sortBuffer[src]);
    }

    playbackSize = 2 * noteBuffer->size();
    //printf("leaving with playback size = %d\n", playbackSize);
    for (int i = 0; i < playbackSize; ++i) {
        // printf("* final output[% d] = % f\n", i, playbackBuffer[i]);
    }
}

void ArpegPlayer::refillPlaybackDOWNUP() {
    copyAndSort();
    //printf("update, there are %d\n", noteBuffer->size());
    // first the "down" part
    for (int i = 0; i < noteBuffer->size(); ++i) {
        const int src = i;
        const int dest = -1 + noteBuffer->size() - i;
        assert(dest >= 0);
        playbackBuffer[dest] = sortBuffer[src];
        printf("copy %d to %d (%f)\n", src, dest, sortBuffer[src]);
    }
    const int upwardEntries = noteBuffer->size() - 2;
    //printf("after down, upward entries = %d\n", upwardEntries);
    if (upwardEntries <= 0) {
        playbackSize = noteBuffer->size();
        //  playbackIndex = 0;
        printf("leaving on no upward\n");
        return;
    }

    for (int i = 0; i < upwardEntries; ++i) {
        // this src can't be right
        const int src = 1 + i;
        const int dest = i + noteBuffer->size();
        playbackBuffer[dest] = sortBuffer[src];
        //printf("copy %d to %d (%f)\n", src, dest, sortBuffer[src]);
    }

    playbackSize = -2 + 2 * noteBuffer->size();
    //printf("leaving with playback size = %d\n", playbackSize);
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
    //printf("refillPlaybackINSIDE_OUT, there are %d med=%d\n", noteBuffer->size(), medianIndex);

    int lowIndex = medianIndex;
    int highIndex = medianIndex;
    playbackBuffer[0] = sortBuffer[medianIndex];
    //printf("copied first entry(0) from %d val %f\n", medianIndex, playbackBuffer[0]);

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
                //printf("copied high src=%d dest=%d val=%f\n", src, dest, playbackBuffer[dest]);
            }
        }
        if (lowIndex >= 0) {
            done = false;
            const int src = lowIndex;
            const int dest = destIndex++;
            playbackBuffer[dest] = sortBuffer[src];
            //printf("copied low src=%d dest=%d val=%f\n", src, dest, playbackBuffer[dest]);
        }
        if (!isOdd) {
            if (highIndex < siz) {
                done = false;
                const int src = highIndex;
                const int dest = destIndex++;
                playbackBuffer[dest] = sortBuffer[src];
                //printf("copied high src=%d dest=%d val=%f\n", src, dest, playbackBuffer[dest]);
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
    //printf("refillPlaybackOUTSIDE_IN, there are %d med=%d isOdd=%d\n", noteBuffer->size(), medianIndex, isOdd);


    int lowIndex = 0;
    int highIndex = siz - 1;
    // playbackBuffer[0] = sortBuffer[medianIndex];
    // printf("copied first entry(0) from %d val %f\n", medianIndex, playbackBuffer[0]);

    int destIndex = 0;
    // for (bool done = false; !done;) {
    for (int i = 0; i < siz / 2; ++i) {
        //printf("in out loop, i=%d\n", i);

        int src = highIndex;
        int dest = destIndex++;
        playbackBuffer[dest] = sortBuffer[src];
        //printf("copied high src=%d dest=%d val=%f\n", src, dest, playbackBuffer[dest]);

        src = lowIndex;
        dest = destIndex++;
        playbackBuffer[dest] = sortBuffer[src];
        //printf("copied low src=%d dest=%d val=%f\n", src, dest, playbackBuffer[dest]);

        --highIndex;
        ++lowIndex;
    }
    if (isOdd) {
        //printf("need to copy middle?\n");
        playbackBuffer[destIndex] = sortBuffer[medianIndex];
        //printf("copied last dest=%d src= %d val %f\n", destIndex, medianIndex, playbackBuffer[destIndex]);
    }
}

void ArpegPlayer::refillPlaybackORDER_PLAYED() {
    const int siz = noteBuffer->size();
    playbackSize = siz;
    for (int i = 0; i < siz; ++i) {
        playbackBuffer[i] = (i + noteBuffer->begin())->cv;
        // printf("sorted input[%d] = %f\n", i, sortBuffer[i]);
    }
}

void ArpegPlayer::refillPlaybackREPEAT_TOP() {
    copyAndSort();
    const int siz = noteBuffer->size();
    const int end = siz - 1;
    int dest = 0;
    //printf("in rep bottom, siz=%d\n", siz);
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
        //printf("in loop, i=%d\n", i);
        dest = i * 2;
        playbackBuffer[dest] = sortBuffer[end];
       // printf("copyed 0 to %d (%f)\n", dest, sortBuffer[end]);

        int src = i;
        ++dest;
        playbackBuffer[dest] = sortBuffer[src];
        //printf("copyed %d to %d (%f)\n", src, dest, sortBuffer[src]);
        assert(src < siz);
    }
    playbackSize = dest + 1;
    // printf("playback size = %d\n", playbackSize);
}

void ArpegPlayer::refillPlaybackREPEAT_BOTTOM() {
    copyAndSort();
    const int siz = noteBuffer->size();
    int dest = 0;
    //printf("in rep bottom, siz=%d\n", siz);
    if (siz == 0) {
        playbackSize = 0;
        // playbackIndex = 0;
        return;
    }
    if (siz == 1) {
        playbackSize = 1;
        playbackBuffer[0] = sortBuffer[0];
        // playbackIndex = 0;
        return;
    }
    for (int i = 0; i < siz - 1; ++i) {
        //printf("in loop, i=%d\n", i);
        dest = i * 2;
        playbackBuffer[dest] = sortBuffer[0];
        //printf("copyed 0 to %d (%f)\n", dest, sortBuffer[0]);

        int src = 1 + i;
        ++dest;
        playbackBuffer[dest] = sortBuffer[src];
        //printf("copyed %d to %d (%f)\n", src, dest, sortBuffer[src]);
        assert(src < siz);
    }
    playbackSize = dest + 1;
    //printf("playback size = %d\n", playbackSize);
}