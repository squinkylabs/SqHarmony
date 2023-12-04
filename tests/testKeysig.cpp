
#include "KeysigOld.h"
#include "asserts.h"

static void testCinC() {
    KeysigOldPtr ks = std::make_shared<KeysigOld>(Roots::C);
    StylePtr style = std::make_shared<Style>();

    Options op(ks, style);
    HarmonyNote hn(op);
    hn.setPitchDirectly(HarmonyNote::C3);
    auto srn = ks->getScaleDeg(hn);
    assertEQ(srn, 1);  // c is the first degree of C
}

static void testAllInC() {
    KeysigOldPtr ks = std::make_shared<KeysigOld>(Roots::C);
    StylePtr style = std::make_shared<Style>();

    Options op(ks, style);
    HarmonyNote hn(op);
    for (int i = 0; i < 17; ++i) {
        hn.setPitchDirectly(HarmonyNote::C3 + i);
        auto srn = ks->getScaleDeg(hn);
        switch (i) {
            case 0:  // c is root
            case 12:
                assertEQ(srn, 1);
                break;
            case 1:  // c# is not in scale
            case 13:
            case 3:       // d#
            case 3 + 12:  // d#
            case 6:       // f#
            case 8:       // g#
            case 10:      // a#
                assertEQ(srn, 0);
                break;
            case 2:  // d
            case 2 + 12:
                assertEQ(srn, 2);
                break;
            case 4:  // e
            case 12 + 4:
                assertEQ(srn, 3);
                break;
            case 5:  // f
                assertEQ(srn, 4);
                break;
            case 7:  // g
                assertEQ(srn, 5);
                break;
            case 9:  // a
                assertEQ(srn, 6);
                break;
            case 11:  // b
                assertEQ(srn, 7);
                break;
            default:
                assert(false);
        }
    }
}

static void testStyle1() {
    auto style = std::make_shared<Style>();

    assert(style->getInversionPreference() == Style::InversionPreference::DISCOURAGE_CONSECUTIVE);
    style->setInversionPreference(Style::InversionPreference::DISCOURAGE);
    assert(style->getInversionPreference() == Style::InversionPreference::DISCOURAGE);
    style->setInversionPreference(Style::InversionPreference::DONT_CARE);
    assert(style->getInversionPreference() == Style::InversionPreference::DONT_CARE);

    assert(style->getRangesPreference() == Style::Ranges::VOCAL_RANGE);
    style->setRangesPreference(Style::Ranges::ENCOURAGE_CENTER);
    assert(style->getRangesPreference() == Style::Ranges::ENCOURAGE_CENTER);

    assert(style->getNoNotesInCommon() == true);
    style->setNoNotesInCommon(false);
    assert(style->getNoNotesInCommon() == false);
}

static void testStyle2() {
    auto style = std::make_shared<Style>();
    style->setRangesPreference(Style::Ranges::WIDE_RANGE);
    // assertLT(blw, style2->minBass());
    const int blw = style->minBass();
    const int bhw = style->maxBass();
    const int tlw = style->minTenor();
    const int thw = style->maxTenor();
    const int alw = style->minAlto();
    const int ahw = style->maxAlto();
    const int slw = style->minSop();
    const int shw = style->maxSop();

    auto style2 = std::make_shared<Style>();
    style2->setRangesPreference(Style::Ranges::VOCAL_RANGE);
    assertLT(blw, style2->minBass());
    assertGT(bhw, style2->maxBass());
    assertLT(tlw, style2->minTenor());
    assertGT(thw, style2->maxTenor());
    assertLT(alw, style2->minAlto());
    assertGT(ahw, style2->maxAlto());
    assertLT(slw, style2->minSop());
    assertGT(shw, style2->maxSop());
}

static void validateRange(StylePtr style) {
    // at least one semi of overlap
    assertGT(style->maxBass(), style->minTenor());
    assertGT(style->maxTenor(), style->minAlto());
    assertGT(style->maxAlto(), style->minSop());
}

void validateRanges() {
    auto style = std::make_shared<Style>();
    validateRange(style);
    style->setRangesPreference(Style::Ranges::ENCOURAGE_CENTER);
    validateRange(style);
    style->setRangesPreference(Style::Ranges::VOCAL_RANGE);
    validateRange(style);
}

static void testGetSet() {
    KeysigOld ks(Roots::C);

    const MidiNote mn(3);
    ks.set(mn, Scale::Scales::Dorian);
    auto x = ks.get();
    assertEQ(x.first.get(), mn.get());
    assert(x.second == Scale::Scales::Dorian);
}

void testKeysig() {
    testCinC();
    testAllInC();
    testStyle1();
    testStyle2();
    validateRanges();
    testGetSet();
}