
#include "Scale.h"
#include "SharpsFlatsPref.h"
#include "asserts.h"

static void testFloat2Pref() {
    assert(float2Pref(0) == UIPrefSharpsFlats::DefaultPlusSharps);
    assert(float2Pref(.49) == UIPrefSharpsFlats::DefaultPlusSharps);
    assert(float2Pref(1) == UIPrefSharpsFlats::DefaultPlusFlats);
    assert(float2Pref(.51) == UIPrefSharpsFlats::DefaultPlusFlats);
    assert(float2Pref(2) == UIPrefSharpsFlats::Sharps);
    assert(float2Pref(3) == UIPrefSharpsFlats::Flats);
}


void testResolveSharpPrefCMajor() {
    Scale scale( MidiNote(MidiNote::C), Scale::Scales::Major);
    assertEQ(resolveSharpPref(UIPrefSharpsFlats::DefaultPlusSharps, scale), true );
    assertEQ(resolveSharpPref(UIPrefSharpsFlats::DefaultPlusFlats, scale), false );
    assertEQ(resolveSharpPref(UIPrefSharpsFlats::Sharps, scale), true );
    assertEQ(resolveSharpPref(UIPrefSharpsFlats::Flats, scale), false );
}

void testResolveSharpPrefGMajor() {
    Scale scale( MidiNote(MidiNote::G), Scale::Scales::Major);
    assertEQ(resolveSharpPref(UIPrefSharpsFlats::DefaultPlusSharps, scale), true );
    assertEQ(resolveSharpPref(UIPrefSharpsFlats::DefaultPlusFlats, scale), true );
    assertEQ(resolveSharpPref(UIPrefSharpsFlats::Sharps, scale), true );
    assertEQ(resolveSharpPref(UIPrefSharpsFlats::Flats, scale), false );
}

void testResolveSharpPrefBFlatMajor() {
    Scale scale( MidiNote(MidiNote::B - 1), Scale::Scales::Major);
    assertEQ(resolveSharpPref(UIPrefSharpsFlats::DefaultPlusSharps, scale), false );
    assertEQ(resolveSharpPref(UIPrefSharpsFlats::DefaultPlusFlats, scale), false );
    assertEQ(resolveSharpPref(UIPrefSharpsFlats::Sharps, scale), true );
    assertEQ(resolveSharpPref(UIPrefSharpsFlats::Flats, scale), false );
}

void testSharpsFlatsPref() {
    testFloat2Pref();
    testResolveSharpPrefCMajor();
    testResolveSharpPrefGMajor();
     testResolveSharpPrefBFlatMajor();

    // assert(false);
}