
#include "FloatNote.h"
#include "NoteConvert.h"
#include "Scale.h"
#include "ScaleQuantizer.h"
#include "asserts.h"

static void testCinC() {
    auto quantizer = ScaleQuantizer::makeTestCMaj();
    auto note = quantizer->run(0);
    assertEQ(note.get(), MidiNote::C3 + 12);  // C4 is float 0
}

static void testDinC() {
    auto quantizer = ScaleQuantizer::makeTestCMaj();

    auto note = quantizer->run(2.f / 12.f);
    assertEQ(note.get(), MidiNote::C3 + 12 + 2);
}

static void testCinC2() {
    auto quantizer = ScaleQuantizer::makeTestCMaj();
    auto note = quantizer->run(1);
    assertEQ(note.get(), MidiNote::C3 + 2 * 12);
}

static void testFull(int baseMidiPitch, Scale::Scales scale) {
    auto quantizer = ScaleQuantizer::makeTest(baseMidiPitch, scale);
    for (float x = -5; x < 5; x += 1.f / 12.f) {
        auto note = quantizer->run(x);
        FloatNote fn;
        NoteConvert::m2f(fn, note);

        const float semi = 1.f / 12.f;
        assertClose(fn.get(), x, semi + .001f);
    }
}

static void testFullC() {
    testFull(MidiNote::C, Scale::Scales::Major);
}

static void testFullG() {
    testFull(MidiNote::C + 7, Scale::Scales::Major);
}

static void testFullGm() {
    testFull(MidiNote::C + 7, Scale::Scales::Minor);
}

static void testUpAndDownStep() {
    auto quantizer = ScaleQuantizer::makeTestCMaj();
    auto note = quantizer->run(0);
    auto note2 = quantizer->run(1.f / 25.f);
    auto not3 = quantizer->run(0);
    assert(false);
}

void testScaleQuantizer() {
    testCinC();
    testCinC2();
    testDinC();
    testFullC();
    testFullG();
    testFullGm();
    // testUpAndDownStep();
}