
#pragma once

#include "Scale.h"

class PESConverter {
public:
    class PESOutput {
    public:
        bool valid = false;
        Scale::Scales mode = Scale::Scales::Major;
        int keyRoot = 0;
    };

    static PESOutput convertToPES(Input& input, bool acceptNonDiatonic);
    static void outputPES(Output& output, const PESOutput& params);
};

inline PESConverter::PESOutput PESConverter::convertToPES(Input& input, bool acceptNonDiatonic) {
    if (input.channels < 1) {
        PESOutput ret;  // unconnected
        ret.valid = true;
    }
    if (input.channels < 12) {
        // wrong number of channels - error
        // TBase::lights[PES_INVALID_LIGHT].value = 8;
        return PESOutput();
    }
    Scale::Role roles[13];
    bool haveAddedRoot = false;
    for (int i = 0; i < 12; ++i) {
        float v = input.getVoltage(i);
        Scale::Role role;
        if (v < 4) {
            role = Scale::Role::NotInScale;
        } else if (v < 9) {
            role = Scale::Role::InScale;
        } else {
            // Only add the first root. Some modules send all note as 10V.
            role = haveAddedRoot ? Scale::Role::InScale : Scale::Role::Root;
            haveAddedRoot = true;
        }
        roles[i] = role;
    }
    roles[12] = Scale::Role::End;
    PESOutput returnValue;

    const auto scaleConverted = Scale::convert(roles, !acceptNonDiatonic);
    if (std::get<0>(scaleConverted) == false) {
        returnValue.valid = false;
    } else {
        returnValue.valid = true;
        returnValue.keyRoot = std::get<1>(scaleConverted).get();
        returnValue.mode = std::get<2>(scaleConverted);
    }
    return returnValue;
}

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