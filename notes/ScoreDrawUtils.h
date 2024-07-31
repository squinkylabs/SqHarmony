#pragma once

#include "SqArray.h"

#include <map>
#include <memory>

class ScoreDrawUtils;
using ScoreDrawUtilsPtr = std::unique_ptr<ScoreDrawUtils>;
class ScoreDrawUtils {
public:
    static ScoreDrawUtilsPtr make();
    void getDrawInfo(const SqArray<int, 16>&);

    class LegerLineInfo {

    };
    std::map<int, LegerLineInfo> _info;
};


