

#include "ScoreDrawUtils.h"


ScoreDrawUtilsPtr ScoreDrawUtils::make() {
    return std::make_unique<ScoreDrawUtils>();
}

void ScoreDrawUtils::getDrawInfo(const SqArray<int, 16>&) {

}