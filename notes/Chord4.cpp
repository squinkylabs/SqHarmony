#include "Chord4.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <sstream>

#include "KeysigOld.h"
#include "Options.h"
#include "ProgressionAnalyzer.h"
#include "ScaleRelativeNote.h"
#include "SqLog.h"
#include "Style.h"

std::atomic<int> __numChord4{0};

bool _globalShow = false;

/*  Chord4::Chord4(int nRoot)
 */
Chord4::Chord4(const Options& options, int nRoot) : _root(nRoot) {
    __numChord4++;
    assert(_root > 0 && _root < 8);

    for (int i = 0; i < CHORD_SIZE; ++i) {
        _notes.push_back(HarmonyNote(options));
    }
    assert(_notes.size() == CHORD_SIZE);
    // now _notes has 4 notes, they are all the same path - the min pitch specified by the style

    for (int index = 0; index < CHORD_SIZE; index++) {
        while (_notes[index] < options.style->absMinPitch()) {
            ++_notes[index];
        }
        if (!isInChord(options, _notes[index])) {
            bumpToNextInChord(options, _notes[index]);
        }
        // Speed up makeNext by getting this far.
    }

    if (!isChordOk(options)) {
        const bool error = makeNext(options);  // Start on valid chord.
        if (error) {
            assert(!valid);
            return;  // If we can't make a valid chord, signal an error.
        }
    }
    valid = true;
}

// TODO: get rid of this!
Chord4::Chord4() : _root(1) {
    valid = true;
    __numChord4++;
}

Chord4::~Chord4() {
    __numChord4--;
    assert(__numChord4 >= 0);
}

void Chord4::_addRef() {
    __numChord4++;
}

/*  int Chord4::Quality() const
 */
int Chord4::quality(const Options& options, bool fTalk) const {
    assert(valid);
    int ret;
    int nTotalDivergence = divergence(options);

    if (fTalk) printf("Div = %d", nTotalDivergence);

    ret = -nTotalDivergence;
    return ret;
}

/*  int Chord4::Divergence() const
 */
int Chord4::divergence(const Options& options) const {
    int nTotalDivergence = 0;
    int s, a, t, b;
    int target;

    assert(_notes.size() == CHORD_SIZE);

    auto style = options.style;
    target = style->minSop() + style->maxSop();
    target /= 2;
    s = target - _notes[3];
    s *= s;

    target = style->minAlto() + style->maxAlto();
    target /= 2;
    a = target - _notes[2];
    a *= a;

    target = style->minTenor() + style->maxTenor();
    target /= 2;
    t = target - _notes[1];
    t *= t;

    target = style->minBass() + style->maxBass();
    target /= 2;
    b = target - _notes[0];
    b *= b;

    nTotalDivergence = s + a + t + b;

    return nTotalDivergence;
}

/* void Chord4::Print() const
 */

std::string Chord4::getString() const {
    assert(valid);
    std::stringstream s;
    assert(_notes.size() == CHORD_SIZE);

    s << toStringShort();
    s << " Root: ";
    s << _root;
    s << " rank: ";
    s << rank;

    return s.str();
}

std::string Chord4::toStringShort() const {
    // let's print out unfinished chords.
    // assert(valid);
    std::stringstream s;
    assert(_notes.size() == CHORD_SIZE);

    for (int i = 0; i < CHORD_SIZE; i++) {
        s << _notes[i].tellPitchName();
    }
    return s.str();
}

#ifdef _DEBUG
void Chord4::dump() const {
    print();
}
#endif

void Chord4::print() const {
    auto str = getString();
    SQINFO("%s", str.c_str());
    // std::cout << str;
}

Chord4Ptr Chord4::fromString(const Options& options, int degree, const char* target) {
    assert(strlen(target) == 8);
    if (options.style->allowVoiceCrossing()) assert(false);     // Not supported ATM.
    const std::string sTarget = target;
    for (int voiceN=TENOR; voiceN <= SOP; voiceN++) {
        const std::string subLow = sTarget.substr(2 * (voiceN-1), 2);
        const std::string subHigh = sTarget.substr(2 * voiceN, 2);

        const int pitchLow = PitchKnowledge::pitchFromName(subLow);
        const int pitchHigh = PitchKnowledge::pitchFromName(subHigh);
        SQINFO("fromString %s on vx %d have %d, %d", target, voiceN, pitchLow, pitchHigh);

        // TODO: assert
        //assert(_notes[voiceN] > _notes[]
    }
    Chord4Ptr chord = std::make_shared<Chord4>(options, degree);
    while (true) {
        if (chord->toStringShort() == target) {
            return chord;
        }
        bool error = chord->makeNext(options);
        if (error) {
            return nullptr;
        }
    }
}

/* void Chord4::BumpToNextInChord(Note note)
 */
void Chord4::bumpToNextInChord(const Options& options, HarmonyNote& note) {
    const bool b = false;

    if (b && false) {
        printf("bump to next called with >%s<\n", toString().c_str());
        printf("%d %d %d %d\n", (int)_notes[0], (int)_notes[1], (int)_notes[2], (int)_notes[3]);
    }
    while (!isInChord(options, note)) ++note;

    if (b && false) {
        printf("leaving bump with %s\n", toString().c_str());
    }
}

/* bool Chord4::inc()
 */
bool Chord4::inc(const Options& options) {
    int nVoice;
    bool fRet = false;  // assume no error

    assert(_notes.size() == CHORD_SIZE);

    ++_notes[CHORD_SIZE - 1];  // inc to next pitch
    bumpToNextInChord(options, _notes[CHORD_SIZE - 1]);

    for (nVoice = CHORD_SIZE - 1; nVoice >= 0; nVoice--) {
        if (_notes[nVoice].isTooHigh(options))  // If we inced too far
                                                // I.E. this voice is out of range..
        {
            if (_globalShow && (nVoice == 1)) {
                SQINFO("note is too high in voic %d, chord=%s", nVoice, toStringShort().c_str());
                SQINFO("pitch[0] = %d, pitch[1] = %d", _notes[0], _notes[1]);
            }
            if (nVoice == 0) {
                fRet = true;  // ... and no more to try, then give up
            } else            // We overflowed, but can carry to next voice
            {
                if (!options.style->allowVoiceCrossing())  // If we require that two voices never cross
                                                           // (meaning alto can never be higher than sop)
                {
                    _notes[nVoice] = _notes[nVoice - 1];  // Go as low as possible without cross!
                } else {
                    _notes[nVoice].setMin(options);  // otherwise, reset this to min
                    bumpToNextInChord(options, _notes[nVoice]);
                }
                ++_notes[nVoice - 1];  // ... and carry to next
                if (_globalShow && nVoice == 1) {
                    SQINFO("raw wrap, before bump voice %d to %d now %s", nVoice, _notes[nVoice], toStringShort().c_str());
                }
                bumpToNextInChord(options, _notes[nVoice - 1]);
                if (_globalShow && nVoice == 1) {
                    SQINFO("wrapping voice %d to %d now %s", nVoice, _notes[nVoice], toStringShort().c_str());
                    SQINFO("pitch[0] = %d, pitch[1] = %d", _notes[0], _notes[1]);
                }
            }
        }
    }

    return fRet;
}

bool Chord4::makeNext(const Options& options) {
    bool fDone;
    bool fError;

    for (fDone = fError = false; !fDone;) {
        if (inc(options)) {
            fDone = true;
            fError = true;
        } else {
            if (isChordOk(options)) {
                fDone = true;  // then we are done
            }
        }
    }
    makeSrnNotes(options);  // fill up the srnNotes array with valid stuff

    return fError;
}

void Chord4::makeSrnNotes(const Options& op) {
    int i;

    assert(_notes.size() == CHORD_SIZE);
    for (i = 0; i < CHORD_SIZE; i++) {
        srnNotes[i] = op.keysig->ScaleDeg(_notes[i]);  // compute the scale rel ones for other guys to use
    }
}

bool Chord4::isChordOk(const Options& options) const {
    int i, nPitch;

    bool show = false;
    if (_globalShow) {
        // show = true;
        // looking for "A3C3E3C4"

        const std::string s = this->toStringShort();

        const char bassOct = s[1];
        const char bassPitch = s[0];

        const char tenorOct = s[3];
        const char tenorPitch = s[2];

        // E1 is the first chord I see
        // I see c2
        // I see C3. why not A3?
        // I can get all the way to c4 in the bass no problem.
        // if (bassOct == '4') {
        //     show = true;
        // }

        // I can find C, and E in the bass
        // I can see A1 and A2

        // I don't see A3C3
        // I see **C3

        // looking for "A3C3E3C4"
        if (
            bassPitch == 'A' && bassOct == '3'
            //      bassPitch == 'A' && bassOct == '3'&& tenorPitch == 'C' && tenorOct == '3'
            //  tenorPitch == 'C' && tenorOct == '3'
        ) {
            show = true;
        }
    }

    if (show) {
        SQINFO("enter isChordOK with chord =%s", this->toStringShort().c_str());
    }

    auto style = options.style;
    assert(_notes.size() == CHORD_SIZE);
    if (!style->allowVoiceCrossing())  // If we require that two voices never cross
                                       // (meaning alto can never be higher than sop)
    {
        int nVoice;

        for (nPitch = -1, nVoice = 0; nVoice < CHORD_SIZE; nVoice++) {
            if (_notes[nVoice] < nPitch) {  // If the next voice is NOT higher than the last
                                            // 11/25 allow same pitch.  This is NG! allows all 4 notes the same!
                                            //   if (b) printf("isChordOk not ok at 248\n");
                return false;               // Then we are bad
            } else {
                nPitch = _notes[nVoice];
            }
        }
    } else {
        SQWARN("can't handle crossing");
        return false;
    }

    bool InvOk;
    const auto chordInversion = inversion(options);
    switch (chordInversion) {
        case ROOT_POS_INVERSION:
            InvOk = true;
            break;
        case FIRST_INVERSION:
            InvOk = style->allow1stInversion();
            break;
        case SECOND_INVERSION:
            InvOk = style->allow2ndInversion();
            break;
        default:
            InvOk = false;
    }
    if (!InvOk) {
        return false;
    }

    bool test[128];
    int matches;
    memset(test, 0, sizeof(test));  // clear our test hit array
    for (i = matches = 0; i < CHORD_SIZE; i++) {
        nPitch = _notes[i];  // get the pitch of this chord member
        assert(nPitch >= 0 && nPitch < 128);

        if (test[nPitch]) matches++;  // if someone at this pitch, count us
        test[nPitch] = true;          // mark that we are here
    }
    if (matches > style->maxUnison()) {
        return false;
    }

    for (i = CHORD_SIZE - 1; i >= 0; i--) {
        if (!isInChord(options, _notes[i])) {
            // if (b) printf("isChordOk not ok at 294\n");
            // how can we get down here with notes not in chord?
            return false;
        }
    }

    if (!isAcceptableDoubling(options)) {
        return false;
    }

    if (!pitchesInRange(options)) {
        return false;
    }

    if (options.style->forbidLeadingToneDoubling() || options.style->forbidLeadingToneChordInRootPosition()) {
        // TODO: shouldn't this be moved to isAcceptableDoubling?
        assert(_notes.size() == CHORD_SIZE);

        // count how many leading tones are in this chord.
        int totalLeadingTones = 0;
        for (i = 0; i < CHORD_SIZE; i++) {
            ScaleRelativeNote tempSrn;
            tempSrn = options.keysig->ScaleDeg(_notes[i]);  // compute the scale rel ones for other guys to use
            if (tempSrn.isLeadingTone()) {
                ++totalLeadingTones;
            }
        }

        // If more than one it means we are doubling the leading tone.
        // That's more difficult than we can do - let's forbid it.
        if ((totalLeadingTones > 1) && options.style->forbidLeadingToneDoubling()) {
            return false;
        }

        // If this is the leading tone triad, then it can't be in root position
        if (_root == 7 && chordInversion == ROOT_POS_INVERSION && options.style->forbidLeadingToneChordInRootPosition()) {
            return false;
        }
    }

    // If we got this far, we must be ok;
    return true;
}

bool Chord4::pitchesInRange(const Options& options) const {
    const HarmonyNote* notes = fetchNotes();
    auto style = options.style;

    const int bass = notes[0];
    const int tenor = notes[1];
    const int alto = notes[2];
    const int soprano = notes[3];
    if (bass < style->minBass() || bass > style->maxBass()) {
        return false;
    }
    if (tenor < style->minTenor() || tenor > style->maxTenor()) {
        return false;
    }
    if (alto < style->minAlto() || alto > style->maxAlto()) {
        return false;
    }
    if (soprano < style->minSop() || soprano > style->maxSop()) {
        return false;
    }
    return true;
}

bool Chord4::isAcceptableDoubling(const Options& options) const {
    int nRoots = 0, nThirds = 0, nFifths = 0;
    for (int nVoice = 0; nVoice < CHORD_SIZE; nVoice++)  // loop over all notes in chord
    {
        const HarmonyNote& harmonyNote = _notes[nVoice];
        switch (chordInterval(options, harmonyNote)) {
            case 1:
                nRoots++;
                break;
            case 3:
                nThirds++;
                break;
            case 5:
                nFifths++;
                break;
            default:
                SQWARN("dlb of non-diatonic");
                assert(false);
        }
    }

    //  Acceptable means there is one of each - root, third, fifth.
    return (nRoots > 0) && (nThirds > 0) && (nFifths > 0);
}

bool Chord4::isCorrectDoubling(const Options& options) const {
    assert(isAcceptableDoubling(options));

    bool ret;
    int nVoice;
    int nRoots, nThirds, nFifths;
    int nDoubled = 0;

    assert(_notes.size() == CHORD_SIZE);

    for (nVoice = nRoots = nThirds = nFifths = 0; nVoice < CHORD_SIZE; nVoice++)  // loop over all notes in chord
    {
        switch (chordInterval(options, _notes[nVoice])) {
            case 1:
                nRoots++;
                if (nRoots > 1) nDoubled = nVoice;
                break;
            case 3:
                nThirds++;
                if (nThirds > 1) nDoubled = nVoice;
                break;
            case 5:
                nFifths++;
                if (nFifths > 1) nDoubled = nVoice;
                break;
        }
    }

    switch (inversion(options)) {
        case ROOT_POS_INVERSION:
            ret = (nRoots == 2) && (nThirds == 1) && (nFifths == 1);
            // double root
            // Note: some people say it's ok to double the root or the fifth
            break;
        case FIRST_INVERSION:
            // srn should be made already
            // makeSrnNotes(options);           // fill up the srnNotes array with valid stuff
            if (srnNotes[BASS].isTonal()) {  // if the bass is tonal
                ret = (nRoots == 1) && (nThirds == 2) && (nFifths == 1);
                // then double the bass (3rd)
            } else {                                   // if bass not tonal...
                ret = srnNotes[nDoubled].isTonal() &&  // double tonal
                      (nRoots > 0) && (nThirds > 0) && (nFifths > 0);
                // make sure at least one of everything
            }
            break;
        case SECOND_INVERSION:
            ret = (nFifths == 2);

            // 11.5.2023. It looks like we let through these crazy doublings? No, I guess the initial check for "acceptable doubling"
            // caught that case.
            assert(ret == false || (nRoots == 1 && nThirds == 1));
            break;
        default:
            static bool shown = false;
            if (!shown) {
                printf("no rule for doubling this inversion. will assume ok\n");
                shown = true;
            }
            assert(false);
            ret = false;
    }
    return ret;
}

#if 0
bool Chord4::isStdDoubling(const Options& options) {
    bool ret;
    int nVoice;
    int nRoots, nThirds, nFifths;
    int nDoubled;

#if 1
    if (this->toStringShort() == "E2G2C3E3") {
        printf("here it is\n");
    }
#endif

    assert(_notes.size() == CHORD_SIZE);

    for (nVoice = nRoots = nThirds = nFifths = 0; nVoice < CHORD_SIZE; nVoice++)  // loop over all notes in chord
    {
        switch (chordInterval(options, _notes[nVoice])) {
            case 1:
                nRoots++;
                if (nRoots > 1) nDoubled = nVoice;
                break;
            case 3:
                nThirds++;
                if (nThirds > 1) nDoubled = nVoice;
                break;
            case 5:
                nFifths++;
                if (nFifths > 1) nDoubled = nVoice;
                break;
        }
    }

    switch (inversion(options)) {
        case ROOT_POS_INVERSION:
            ret = (nRoots == 2) && (nThirds == 1) && (nFifths == 1);
            // double root
            // Note: some people say it's ok to double the root or the fifth
            break;
        case FIRST_INVERSION:
            makeSrnNotes(options);           // fill up the srnNotes array with valid stuff
            if (srnNotes[BASS].isTonal()) {  // if the bass is tonal
                ret = (nRoots == 1) && (nThirds == 2) && (nFifths == 1);
                // then double the bass (3rd)
            } else {                                   // if bass not tonal...
                ret = srnNotes[nDoubled].isTonal() &&  // double tonal
                      (nRoots > 0) && (nThirds > 0) && (nFifths > 0);
                // make sure at least one of everything
            }
            break;
        default:
            static bool shown = false;
            if (!shown) {
                printf("no rule for doubling this inversion. will assume ok\n");
                shown = true;
            }
            ret = true;
    }
    return ret;
}
#endif

/* ChordRelativeNote Chord4::ChordInterval(Note note)
 */
ChordRelativeNote Chord4::chordInterval(const Options& options, HarmonyNote note) const {
    // static int dumb = -1;
    int nt = 0;  // assume bogus
    ChordRelativeNote ret;

    ScaleRelativeNote srnN;

    srnN = options.keysig->ScaleDeg(note);  // get scale degree
    if (srnN.isValid()) {
        nt = 1 + srnN - _root;  // to go from scale rel to chord rel, just normalize to chord root
        if (nt <= 0) nt += 7;   // keep positive!
    }
    ret.set(nt);
    return ret;
}

ChordRelativeNote Chord4::chordInterval(const Options& options, int voiceNumber) const {
    return chordInterval(options, _notes[voiceNumber]);
}

bool Chord4::isInChord(const Options& options, HarmonyNote test) const {
    bool ret = false;

    // const bool b = (this->toStringShort() == "E2A2C3A3");

    const ChordRelativeNote crnX = chordInterval(options, test);
    switch (crnX) {
        case 1:
        case 3:
        case 5:
            ret = true;
    }

    return ret;
}

/** int Chord4::Inversion()
 *
 */
INVERSION Chord4::inversion(const Options& options) const {
    INVERSION ret;

    switch (chordInterval(options, _notes[0])) {
        case 1:
            ret = ROOT_POS_INVERSION;
            break;
        case 3:
            ret = FIRST_INVERSION;
            break;
        case 5:
            ret = SECOND_INVERSION;
            break;
        default:
            ret = NO_INVERSION;
    }

    return ret;
}

/* bool Chord4::CanFollowThisGuy(const Chord4 * const ThisGuy) const;
 */
#if 0
bool Chord4::canFollowThisGuy(const Options& options, const Chord4& thisGuy) const {
    ProgressionAnalyzer analyzer(thisGuy, *this, false);

    assert(false);
    return false;
   // return analyzer.isLegal(options);
}
#endif

int Chord4::penaltForFollowingThisGuy(const Options& options, int lowestPenaltySoFar, const Chord4* thisGuy, bool show) const {
    assert(valid);
    assert(thisGuy->valid);
    if (show) {
        SQINFO("enter Chord4::penaltForFollowingThisGuy");
    }
    ProgressionAnalyzer analyzer(thisGuy, this, show);
    return analyzer.getPenalty(options, lowestPenaltySoFar);
}
