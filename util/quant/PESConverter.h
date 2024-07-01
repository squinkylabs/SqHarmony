
#pragma once

#include "Scale.h"
//#include "TestComposite.h"

class PESConverter {
public:
    class PESOutput {
    public:
        bool valid = false;
        Scale::Scales mode = Scale::Scales::Major;
        int keyRoot = 0;
    };

    static PESOutput convertToPES(Input& input);
    static void outputPES(Output& output, const PESOutput& params);
};

inline PESConverter::PESOutput PESConverter::convertToPES(Input& input) {
      if (input.channels < 1) {
        PESOutput ret;   // unconnected
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
            // only add the first root. Some modules send all note as 10V.
            role = haveAddedRoot ? Scale::Role::InScale : Scale::Role::Root;
            haveAddedRoot = true;
        }
        roles[i] = role;
    }
    roles[12] = Scale::Role::End;
    PESOutput returnValue;

    const auto scaleConverted = Scale::convert(roles, false);
    if (std::get<0>(scaleConverted) == false) {
        //TBase::lights[PES_INVALID_LIGHT].value = 8;
        returnValue.valid = false;
    } else {
       // TBase::lights[PES_INVALID_LIGHT].value = 0;
       returnValue.valid = true;
        //TBase::params[KEY_PARAM].value = std::get<1>(scaleConverted).get();
        returnValue.keyRoot = std::get<1>(scaleConverted).get();
       // TBase::params[MODE_PARAM].value = int(std::get<2>(scaleConverted));
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