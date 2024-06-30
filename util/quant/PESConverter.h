
#pragma once

#include "Scale.h"
#include "TestComposite.h"

class PESConverter {
public:
    //   TBase::lights[PES_INVALID_LIGHT].value = 0;
    //     TBase::params[KEY_PARAM].value = std::get<1>(scaleConverted).get();
    //    TBase::params[MODE_PARAM].value = int(std::get<2>(scaleConverted));
    class PESOutput {
    public:
        bool valid = false;
        Scale::Scales mode = Scale::Scales::Major;
        int keyRoot = 0;
    };

    static PESOutput convertToPES(const Input& input);
    static void outputPES(Output& output, const PESOutput& params);
};

inline void PESConverter::outputPES(Output& output, const PESOutput& params) {
    if (output.isConnected()) {
        output.channels = 12;
    }

    const auto note = MidiNote(MidiNote::C + params.keyRoot);
    const auto mode = params.mode;
    const Scale::RoleArray roleArray = Scale::convert(note, mode);
    for (int i = 0; i < 12; ++i) {
        float v = 0;
        switch (roleArray.data[i]) {
            case Scale::Role::InScale:
                v = 8;
                break;
            case Scale::Role::Root:
                v = 10;
                break;
            case Scale::Role::NotInScale:
            case Scale::Role::End:
                break;
            default:
                assert(false);
        }
        output.setVoltage(v, i);
    }
}