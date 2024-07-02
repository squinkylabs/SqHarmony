
#include "Harmony2.h"
#include "Scale.h"
#include "TestComposite.h"
#include "asserts.h"
#include "testUtils.h"

using Comp = Harmony2<TestComposite>;
using CompPtr = std::shared_ptr<Comp>;

static void hookUpCV(Comp& comp) {
    comp.outputs[Comp::PES_OUTPUT].channels = 1;
}

// Will set the key sig and stuff, but won't call process,
// so the composite may not be "settled".
static CompPtr makeComp(const Scale& fromScale) {
    CompPtr composite = std::make_shared<Comp>();
    hookUpCV(*composite);

    const auto x = fromScale.get();
    composite->params[Comp::KEY_PARAM].value = std::get<0>(x).get();
    composite->params[Comp::MODE_PARAM].value = int(std::get<1>(x));
    return composite;
}

static Scale::Role* getRolesCMinor() {
    static Scale::Role roles[] = {
        Scale::Role::Root,  // C
        Scale::Role::NotInScale,
        Scale::Role::InScale,     // D
        Scale::Role::InScale,     // E-
        Scale::Role::NotInScale,  // E
        Scale::Role::InScale,     // F
        Scale::Role::NotInScale,  // G-
        Scale::Role::InScale,     // G
        Scale::Role::InScale,     // A-
        Scale::Role::NotInScale,  // A
        Scale::Role::InScale,     // B-
        Scale::Role::NotInScale,  // B
        Scale::Role::End};
    return roles;
}

static void testPESOutput() {
    Scale scale;
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Minor);
    auto comp = makeComp(scale);
    processOnce(*comp);

    assertEQ(int(comp->outputs[Comp::PES_OUTPUT].channels), 12);
    for (int i = 0; i < 12; ++i) {
        const int x = int(std::round(comp->outputs[Comp::PES_OUTPUT].getVoltage(i)));
        switch (x) {
            case 0:  // not in scale
                assert(getRolesCMinor()[i] == Scale::Role::NotInScale);
                break;
            case 8:
                assert(getRolesCMinor()[i] == Scale::Role::InScale);
                break;
            case 10:
                assert(getRolesCMinor()[i] == Scale::Role::Root);
                break;
            default:
                assert(false);
        }
    }
}

static void testPESInput() {
    Scale scale;
    scale.set(MidiNote(MidiNote::D), Scale::Scales::Dorian);
    auto comp = makeComp(scale);
    auto& input = comp->inputs[Comp::PES_INPUT];
    input.channels = 12;
    input.setVoltage(10, 0);  // C
    input.setVoltage(0, 1);
    input.setVoltage(8, 2);   // D
    input.setVoltage(8, 3);   // E-
    input.setVoltage(0, 4);   // E
    input.setVoltage(8, 5);   // F
    input.setVoltage(0, 6);   // G-
    input.setVoltage(8, 7);   // G
    input.setVoltage(8, 8);   // A-
    input.setVoltage(0, 9);   // A
    input.setVoltage(8, 10);  // B-
    input.setVoltage(0, 11);  // B

    processOnce(*comp);

    assertLT(comp->lights[Comp::PES_INVALID_LIGHT].value, 5);  // No error.
    assertEQ(int(comp->params[Comp::KEY_PARAM].value), MidiNote::C);
    assertEQ(int(std::round(comp->params[Comp::MODE_PARAM].value)), int(Scale::Scales::Minor));
}

static void testRoundTrip() {
    Scale scale;
    scale.set(MidiNote(MidiNote::C), Scale::Scales::Minor);
    auto compSetter = makeComp(scale);
    auto& setterOutput = compSetter->outputs[Comp::PES_OUTPUT];

    scale.set(MidiNote(MidiNote::D), Scale::Scales::Dorian);
    auto compGetter = makeComp(scale);
    auto& getterInput = compGetter->inputs[Comp::PES_INPUT];
    getterInput.channels = 12;

    processOnce(*compSetter);
    for (int i = 0; i < 12; ++i) {
        float f = setterOutput.getVoltage(i);
        getterInput.setVoltage(f, i);
    }
    processOnce(*compGetter);

    assertLT(compGetter->lights[Comp::PES_INVALID_LIGHT].value, 5);  // No error.
    assertEQ(int(compGetter->params[Comp::KEY_PARAM].value), MidiNote::C);
    assertEQ(int(std::round(compGetter->params[Comp::MODE_PARAM].value)), int(Scale::Scales::Minor));
}

void testHarmony2C() {
    testPESOutput();
    testPESInput();
    testRoundTrip();
}

#if 0
void testFirst() {
    SQINFO("Test First");
    // testPESOutput();
    testRoundTrip();
}
#endif