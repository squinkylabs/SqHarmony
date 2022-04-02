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

// int Chord4::size;

int __numChord4 = 0;

/*  Chord4::Chord4(int nRoot)
 */
Chord4::Chord4(const Options& options, int nRoot) : root(nRoot) {
    __numChord4++;
    assert(root > 0 && root < 8);

    for (int i = 0; i < CHORD_SIZE; ++i) {
        _notes.push_back(HarmonyNote(options));
    }
    assert(_notes.size() == CHORD_SIZE);
    // now _notes has 4 notes, they are all the same path - the min pitch specificied by the style

    for (int index = 0; index < CHORD_SIZE; index++) {
        while (_notes[index] < options.style->absMinPitch()) {
            ++_notes[index];
        }
        if (!isInChord(options, _notes[index])) {
            bumpToNextInChord(options, _notes[index]);
        }
        // speed up makeNext by getting this far
    }

    if (!isChordOk(options)) {
        const bool error = makeNext(options);  // Start on valid chord
        if (error) {
            assert(!valid);
            return;  // if we can't make a valid chord, signal an error
        }
    }
    valid = true;
}

// TODO: get rid of this!
Chord4::Chord4() : root(1) {
    valid = true;
    __numChord4++;
}

Chord4::~Chord4() {
    __numChord4--;
    assert(__numChord4 >= 0);
}

void Chord4::addRef() {
    __numChord4++;
}

#if 0
Chord4::Chord4(const Chord4& other) {
    *this = other;
    __numChord4++;
}
#endif

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
    s << root;
    s << " rank: ";
    s << rank;
    

#if 0

    s << "Root: ";
    s << root;
    s << "  ";
    for (int i = 0; i < CHORD_SIZE; i++) {
        s << _notes[i].tellPitchName();
    }
#endif
    return s.str();
}

std::string Chord4::toStringShort() const {
    assert(valid);
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
    // assert(toStringShort() != "E2A2C3A3");
    while (!isInChord(options, note)) ++note;

    if (b && false) {
        printf("leaving bump with %s\n", toString().c_str());
    }
#if 0
    if (toStringShort() == "E2A2C3A3") {
        printf("leaving bump with the magic value\n");
    }
#endif
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
            if (nVoice == 0) {
                fRet = true;  // ... and no more to try, thenb give up
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
                ++_notes[nVoice - 1];  // ... and cary to next
                bumpToNextInChord(options, _notes[nVoice - 1]);
            }
        }
    }

#if 0
    if (root == 6) {
        std::string s = toStringShort();
        if (s == "E2A2C3A3") {
            printf("leaving inc with note at 196\n");
        }
    }
#endif
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
    bool ret = true;
    int i, nPitch;

#if 0
    const bool b = (this->toStringShort() == "E2A2C3A3");
    if (b) {
        printf("entering okChordOK with the note. 221\n");
    }
#endif

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
        printf("can't handle yet\n");
        return false;
    }

    bool InvOk;
    switch (inversion(options)) {
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
#if 0
        if (b) printf("isChordOk not ok at 274\n");
        if (toStringShort() == "E2A2C3A3") {
            printf("failing isChordOk with the magic value for illegal second inversion\n");
        }
#endif
        return false;
    }

    bool test[128];
    int matches;
    memset(test, 0, sizeof(test));  // clear our test hit array
    for (i = matches = 0; i < CHORD_SIZE; i++) {
        nPitch = _notes[i];           // get the pitch of this chord member
        if (test[nPitch]) matches++;  // if someone at this pitch, count us
        test[nPitch] = true;          // mark that we are here
    }
    if (matches > style->maxUnison()) {
        // if (b) printf("isChordOk not ok at 287\n");
        return false;
        // If more unisons in the chord than we allow
    }

    for (i = CHORD_SIZE - 1; i >= 0; i--) {
        if (!isInChord(options, _notes[i])) {
            // if (b) printf("isChordOk not ok at 294\n");
            return false;
        }
    }

#if 1
    if (!isAcceptableDoubling(options)) {
        return false;
    }
#else
    if (style->requireStdDoubling() && !isStdDoubling(options)) {
        // if (b) printf("isChordOk not ok at 300\n");
        //  double root, contain 3 and 5
        return false;
    }
#endif
    if (!pitchesInRange(options)) {
        return false;
    }
    return ret;
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
        switch (chordInterval(options, _notes[nVoice])) {
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

    return (nRoots > 0) && (nThirds > 0) && (nFifths > 0);
}

bool Chord4::isCorrectDoubling(const Options& options) const {
    assert(isAcceptableDoubling(options));

    bool ret;
    int nVoice;
    int nRoots, nThirds, nFifths;
    int nDoubled = 0;

#if 0
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
        nt = 1 + srnN - root;  // to go from scale rel to chord rel, just normalize to chord root
        if (nt <= 0) nt += 7;  // keep positive!
    }
    ret.set(nt);
    return ret;
}

/* bool Chord4::InChord(Note test)
 */
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
#if 0
    if (b) {
        printf("here is note at isInChord, will ret %d\n", ret);
        // assert(false);
    }
#endif
    return ret;
}

/* int Chord4::Inversion()

 */
INVERSION Chord4::inversion(const Options& options) const {
    // static int dumb = -1;
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
#if 0
if (notes[0] != dumb)
    {
    printf("-note %d chord int %d-\n", (int) notes[0], (int) ChordInterval(notes[0]));
    dumb = notes[0];
    }
#endif
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
