
#include "Keysig.h"
#include "asserts.h"

static void testCinC() {
    KeysigPtr ks = std::make_shared<Keysig>(Roots::C);
    StylePtr style = std::make_shared<Style>();

    Options op(ks, style);
    HarmonyNote hn(op);
    hn.setPitchDirectly(HarmonyNote::C3);
    auto srn = ks->ScaleDeg(hn);
    assertEQ(srn, 1);  // c is the first degree of C
}

static void testAllInC() {
    KeysigPtr ks = std::make_shared<Keysig>(Roots::C);
    StylePtr style = std::make_shared<Style>();

    Options op(ks, style);
    HarmonyNote hn(op);
    for (int i = 0; i < 17; ++i) {
        hn.setPitchDirectly(HarmonyNote::C3 + i);
        auto srn = ks->ScaleDeg(hn);
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

static void testStyle() {
    auto style = std::make_shared<Style>();

    assert(style->getInversionPreference() == Style::Inversion::DISCOURAGE_CONSECUTIVE);
    style->setInversionPreference(Style::Inversion::DISCOURAGE);
    assert(style->getInversionPreference() == Style::Inversion::DISCOURAGE);
    style->setInversionPreference(Style::Inversion::DONT_CARE);
    assert(style->getInversionPreference() == Style::Inversion::DONT_CARE);

    assert(style->getRangesPreference() == Style::Ranges::NORMAL_RANGE);
    style->setRangesPreference(Style::Ranges::ENCOURAGE_CENTER);
    assert(style->getRangesPreference() == Style::Ranges::ENCOURAGE_CENTER);
}

void testKeysig() {
    testCinC();
    testAllInC();
    testStyle();
}