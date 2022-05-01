#include <map>
#include <random>

#include "Chord4Manager.h"
#include "HarmonyChords.h"
#include "KeysigOld.h"
#include "Style.h"
#include "asserts.h"

class IntAnalyzer {
public:
    void add(int i) {
        // SQINFO("adding %d", i);
        auto it = data.find(i);
        if (it == data.end()) {
            data[i] = 1;
        } else {
            data[i] += 1;
        }
    }
    void dump() {
        for (auto it : data) {
            int key = it.first;
            int value = it.second;
            SQINFO("count[%d] = %d", key, value);
        }
    }

    // verifies a uniform distribution
    void checkExtremes(int start, int end, float pctTolerance) {
        // dump();
        assertEQ(data.begin()->first, 1);
        assertEQ(data.rbegin()->first, 7);
        const int middle = (start + end) / 2;
        assert(data.find(middle) != data.end());
        const int middleCount = data[middle];
        const int startCount = data[start];
        const int endCount = data[end];

        assertClosePct(startCount, middleCount, pctTolerance);
        assertClosePct(endCount, middleCount, pctTolerance);

        for (auto it : data) {
            const int count = it.second;
            assertClosePct(count, middleCount, pctTolerance);
        }
        for (int i = start; i <= end; ++i) {
            assert(data.find(i) != data.end());
        }
    }

    int highest() const { return data.rbegin()->first; }

    int lowest() const { return data.begin()->first; }

private:
    using container = std::map<int, int>;
    container data;
};

static void testHiLo() {
    IntAnalyzer a;
    a.add(100);
    a.add(200);
    a.add(150);
    assertEQ(a.highest(), 200);
    assertEQ(a.lowest(), 100);
}

static StylePtr makeStyle() {
    return std::make_shared<Style>();
}

static KeysigOldPtr makeKeysig(bool minor) {
    auto ret = std::make_shared<KeysigOld>(Roots::C);
    if (minor) {
        ret->set(MidiNote::C, Scale::Scales::Minor);
    }
    return ret;
}

static Options makeOptions(bool minor) {
    Options o(makeKeysig(minor), makeStyle());
    return o;
}

static Options makeOptions(int root, Scale::Scales scale) {
    auto ks = std::make_shared<KeysigOld>(Roots::C);

    ks->set(root, scale);
    Options o(ks, makeStyle());
    return o;
}

std::function<int()> getRandFunc() {
    static std::mt19937 generator;
    static std::uniform_real_distribution<float> distribution{0, 1.0};

    return []() {
        const float f = distribution(generator);
        const int i = int(std::ceil(7.f * f));
        assert(i > 0 && i < 8);
        return i;
    };
}

static void testGenerator() {
    auto rand = getRandFunc();
    IntAnalyzer a;
    for (int i = 0; i < 10000; ++i) {
        a.add(rand());
    }
    a.checkExtremes(1, 7, 10.f);
}

void testRand(const Options& options) {
    IntAnalyzer penaltyAnalyzer;
    IntAnalyzer rangeAnalyzer;
    //   auto options = makeOptions();
    Chord4Manager mgr(options);

    //SQINFO("min bass = %d, max sop = %d", options.style->minBass(), options.style->maxSop());
    
    if (!mgr.isValid()) {
        SQINFO("can't create manager for this style");
        assert(false);
        return;
    }
    auto rand = getRandFunc();

    int rootA = rand();
    auto chordA = HarmonyChords::findChord(false, options, mgr, rootA);
    //   SQINFO("chord a=%s", chordA->toString().c_str());
    int rootB = rootA;
    while (rootB == rootA) {
        rootB = rand();
        //    SQINFO("b candidate = %d", rootB);
    }
    //  SQINFO("final b = %d", rootB);
    //   SQINFO("will gen with root=%d, prev=%s", rootB, chordA->toString().c_str());
    auto chordB = HarmonyChords::findChord(false, options, mgr, *chordA, rootB);
    assert(chordB);
    //  SQINFO("chord b=%s", chordB->toString().c_str());

    HarmonyChords::ChordHistory* history = nullptr;
    for (int i = 0; i < 1000; ++i) {
        int rootC = rootB;
        while (rootC == rootB) {
            // SQINFO("GOT SAME");
            rootC = rand();
        }
        //  SQINFO("going to try b=%d C=%d", rootB, rootC);
        const Chord4* thirdChord = HarmonyChords::findChord(false, options, mgr, *chordA, *chordB, rootC);
        assert(thirdChord);

        rangeAnalyzer.add(thirdChord->fetchNotes()[3]);
        rangeAnalyzer.add(thirdChord->fetchNotes()[0]);

        /*
            SQINFO("progression a=%s b=%s c=%s",
                   chordA->toString().c_str(),
                   chordB->toString().c_str(),
                   thirdChord->toString().c_str());
        */
        const int penalty = HarmonyChords::progressionPenalty(options, 1000, chordA, chordB, thirdChord, false);
        penaltyAnalyzer.add(penalty);

        chordA = chordB;
        chordB = thirdChord;
        rootB = rootC;
    }
    //SQINFO("Here are penalties");
    //penaltyAnalyzer.dump();

    // SQINFO("Here are pitchextremes");
    // SQINFO("Pitch range = %d to %d", rangeAnalyzer.lowest(), rangeAnalyzer.highest());
}

static void testValid(int dx) {
    auto options = makeOptions(false);
    options.style->setSpecialTestMode(dx);
    Chord4Manager mgr(options);
    if (!mgr.isValid()) {
        SQINFO("make manager failed in testValid");
        return;
    }
    for (int root = 1; root < 8; ++root) {
        bool done = false;
        for (int rank = 0; !done; rank++) {
            auto ch = mgr.get2(root, rank);
            if (!ch) {
                done = true;
            } else {
                assert(ch->isAcceptableDoubling(options));
            }
        }
    }
}

static void testValid() {
    testValid(8);  // this is the magic bad number
    for (int i = 0; i < 10; ++i) {
        testValid(i);
    }
}

static void testRand(int root, Scale::Scales scale) {
    auto options = makeOptions(root, scale);
    SQINFO("normal options");
    testRand(options);
    options.style->setRangesPreference(Style::Ranges::NARROW_RANGE);
    SQINFO("NARROW_RANGE");
    testRand(options);
    options.style->setRangesPreference(Style::Ranges::ENCOURAGE_CENTER);
    SQINFO("ENCOURAGE_CENTER");
    testRand(options);

    // 8 and above fails
    for (int i = 1; i < 8; ++i) {
        // SQINFO("Especial test mode %d", i);
        options.style->setSpecialTestMode(i);
        testRand(options);
    }
}

static void testRand() {
    testRand(1, Scale::Scales::Major);
    testRand(1, Scale::Scales::Minor);
}

void testHarmonyChordsRandom() {
    // SQWARN("put back test valid");
    testValid();
    testGenerator();
    testHiLo();
    testRand();

    assertEQ(__numChord4, 0);
}
