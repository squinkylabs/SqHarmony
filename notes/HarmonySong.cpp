

#include "HarmonySong.h"

#include <assert.h>

#include <iostream>

#include "ProgressionAnalyzer.h"

HarmonySong::HarmonySong(const Options& options, const int* pS) : chordManager(options), firstTime(true) {
    int i;
    bool done;

    for (i = 0, done = false; !done; i++) {
        if (pS[i] == 0) {
            done = true;
        } else {
            auto ch = std::make_shared<RankedChord>(chordManager, pS[i]);
            chords.push_back(ch);
        }
    }
}

void HarmonySong::print() const {
    int nChord;

    assert(this);
    const int size = int(chords.size());
    for (nChord = 0; nChord < size; nChord++) {
        chords[nChord]->print();
        std::cout << std::endl;
    }
}

void HarmonySong::analyze(const Options& options) const {
    const int size = int(chords.size());
    for (int nChord = 1; nChord < size; nChord++) {
        ProgressionAnalyzer* p = new ProgressionAnalyzer(
            chords[nChord - 1]->fetch2(),
            chords[nChord]->fetch2(),
            true);
        bool f = 0 == p->getPenalty(options, ProgressionAnalyzer::MAX_PENALTY);
        if (!f) {
            assert(false);
        }
        delete p;
    }
}

bool HarmonySong::isValid() const {
#if 1
    return true;

#else
    // CAn turn this on to affect evaluation
    const int size = GetSize() for (int i = 1; i < size; i++) {
        Chord4 const* a = chords[i - 1]->Fetch();
        Chord4 const* b = chords[i]->Fetch();

        if (a->FetchNotes()[SOP] == b->FetchNotes()[SOP])  // must have motion!
            return false;
    }
    return true;
#endif
}

bool HarmonySong::Generate(const Options& options, int nStep, bool show) {
    bool done, ret;
    const int size = int(chords.size());
    // if (nStep < 5) printf("Just started level %d\n", nStep);

    chords[nStep]->reset();
#if 0  // tried to spiff up for regenerate
if (nStep == 0)		// if we are starting from top
	{
	TRACE("Enter Generate\n");
	if (FirstTime)
		{
		TRACE("In the first time case\n");
		chords_ar[nStep]->Reset();
		FirstTime = false;
		}
	else
		{
		TRACE("----Not first generate");
		chords_ar[size-1]->MakeNext();		// advance to we generate a new song
		}
	}
#endif

    for (ret = false, done = false; !done;) {
        if (nStep == 0 || (0 < chords[nStep]->penaltyForFollowingThisGuy(options, ProgressionAnalyzer::MAX_PENALTY, *chords[nStep - 1], show))) {
            // if progression is valid to this chord
            // always valid if no prev!

            if (nStep == (size - 1)) {  // if we are the last chord, and are a valid prog

                if (isValid()) {  // is song "nice" ?

                    ret = true;   // then we will be cool!
                    done = true;  // so stop!
                } else {
                    goto TRY_AGAIN;
                }
            } else {
                bool f = Generate(options, nStep + 1, show);  // if not last chord, then go on the next
                if (f) {                                      // if next generated ok, we are recursing back up!

                    ret = true;
                    done = true;
                }
            }
        }
    TRY_AGAIN:
        if (!done) done = chords[nStep]->makeNext();  // if more to do, try next chord in rank
                                                      // if no next, we are screwed!
    }
    // if (nStep == 0) TRACE("Leaving Generate with return %d", ret);
    return ret;
}
