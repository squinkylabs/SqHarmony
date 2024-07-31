#pragma once

#include "SqArray.h"
#include "SharpsFlatsPref.h"

#include <map>
#include <memory>



class Scale;
class ScoreDrawUtils;
using ScoreDrawUtilsPtr = std::unique_ptr<ScoreDrawUtils>;
class ScoreDrawUtils {
public:
    static ScoreDrawUtilsPtr make();
    // (scale, input, bassStaff, pref
    void getDrawInfo(
        const Scale& scale,
        const SqArray<int, 16>& input,
        bool bassStaff,
        UIPrefSharpsFlats pref);

    class LegerLineInfo {
    };
    std::map<int, LegerLineInfo> _info;
};


