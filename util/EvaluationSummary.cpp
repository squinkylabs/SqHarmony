

#include "EvaluationSummary.h"

#include <cstring>

#include "MelodyEvaluator.h"
#include "MelodyGenerator.h"

std::string EvaluatorResult::toString() const {
    std::stringstream s;
    s << "[" << int(this->rule);
    s << ", " << this->score << "]";

    return s.str();
}

std::string EvaluationSummary::toString() const {
    std::string ret = "(rule, score): ";
    for (int i = 0; i < numStyles; ++i) {
        ret += this->results[i].toString();
    }
    return ret;
}

EvaluationSummary EvaluationSummary::fromRows(const MelodyRow* pRows, unsigned numRows, const MelodyMutateStyle& style, unsigned worstRow) {
    assert(worstRow < numRows);
    EvaluationSummary ret;
    // for (unsigned i = 0; i < numRows; ++i) {
    //     const MelodyRow& row = pRows[i];

    //     // TODO: we should really combine this... need a new test.
    //     // Actually: don't we just return the summary from the worst row?
    //     ret = fromRow(row, style);
    // }
    const MelodyRow& worst = pRows[worstRow];
    ret = fromRow(worst, style);
    return ret;
}

EvaluationSummary EvaluationSummary::fromRow(const MelodyRow& row, const MelodyMutateStyle& style) {
    MelodyEvaluator::getPenalty(row, style);

    EvaluationSummary ret;

    unsigned index = (unsigned int)(Styles::OnlySeekCenter);
    ret.results[index].rule = Styles::OnlySeekCenter;
    ret.results[index].score = MelodyEvaluator::nonCenteredPenalty(row, style);

    index = (unsigned int)(Styles::OnlySeekRange);
    ret.results[index].rule = Styles::OnlySeekRange;
    ret.results[index].score = MelodyEvaluator::pitchRangePenalty(row, style);

    index = (unsigned int)(Styles::OnlyDiscorageLeaps);
    ret.results[index].rule = Styles::OnlyDiscorageLeaps;
    ret.results[index].score = MelodyEvaluator::leapsPenalty(row, style);

    index = (unsigned int)(Styles::OnlyDissonant);
    ret.results[index].rule = Styles::OnlyDissonant;
    ret.results[index].score = MelodyEvaluator::disonnantPenalty(row, style);

    index = (unsigned int)(Styles::OnlyUnison);
    ret.results[index].rule = Styles::OnlyUnison;
    ret.results[index].score = MelodyEvaluator::unisonsPenalty(row, style);

    assert(int(Styles::OnlyUnison) == (int(Styles::Disabled) - 1));

  
    //std::sort(ret.results, ret.results + int(Styles::OnlyDissonant) + 1, f);
    SQINFO("from row, before sort: %s", ret.toString().c_str());
    ret.sort();
    SQINFO("from row, after  sort: %s", ret.toString().c_str());
    ret.assertValid();
    return ret;
}

void EvaluationSummary::sort() {
      std::function<bool(const EvaluatorResult&, const EvaluatorResult&)> f = [](const EvaluatorResult& a, const EvaluatorResult& b) {
        return a.score > b.score;
    };
    std::sort(results, results + numStyles, f);
}
void EvaluationSummary::combine(EvaluationSummary& inOut, const EvaluationSummary& in) {
    inOut.assertValid();
    in.assertValid();

   // SQINFO("inOut = %s", inOut.toString().c_str());
  //  SQINFO("in = %s", in.toString().c_str());

    // loop through everything in in
    for (int i = 0; i < numStyles; ++i) {
      //  SQINFO("dealing with  in %d", i);
        const auto& inResult = in.results[i];
        // If it has a valid setting
        if (inResult.rule != Styles::Disabled) {
            bool placed = false;
            // look for a place in outResult that has the same rule
            for (int i = 0; i < numStyles; ++i) {
                //SQINFO("looking to place output %d", i);
                auto& outResult = inOut.results[i];
                if (outResult.rule == inResult.rule) {
                    outResult.score = std::max(outResult.score, inResult.score);
                    //SQINFO("combing scores");
                    assert(!placed);
                    placed = true;
                }
            }
            if (!placed) {
                for (int i = 0; i < numStyles; ++i) {
                   // SQINFO("not combined, so looking to copy to disabled %d", i);
                    auto& outResult = inOut.results[i];
                    if (outResult.rule == Styles::Disabled) {
                        // inOutl.results = inResult.results;
                        outResult = inResult;
                      //  SQINFO("copying to output");
                        placed = true;
                        break;
                    }
                }
            }
        }
    }
    inOut.sort();
    //SQINFO("after inOut = %s", inOut.toString().c_str());
    inOut.assertValid();
}

#if 0  // version 1
void EvaluationSummary::combine(EvaluationSummary& inOut, const EvaluationSummary& in) {
    inOut.assertValid();
    in.assertValid();

    SQINFO("inOut = %s", inOut.toString().c_str());
    SQINFO("in = %s", in.toString().c_str());

    for (int i = 0; i < numStyles; ++i) {
        // TODO: this is usually wrong
        inOut.results[i].score = std::max(inOut.results[i].score, in.results[i].score);
    }
    SQINFO("after inOut = %s", inOut.toString().c_str());

    inOut.assertValid();
}
#endif

void EvaluationSummary::assertValid() const {
    if (!_checkValid(false)) {
        SQINFO("bad summary: %s", this->toString().c_str());
        _checkValid(true);
    }
}
bool EvaluationSummary::isValid() const {
    return _checkValid(false);
}

bool EvaluationSummary::_checkValid(bool doAssert) const {
    bool seenEnabled = false;

    // for (unsigned i = 0; i < numStyles; ++i) {
    //     const auto result = this->results[i];
    //     SQINFO("in AV result[%d] rule=%d score=%f", i, result.rule, result.score);
    // }
    for (int i = (numStyles - 1); i >= 0; --i) {
        const auto result = this->results[i];
        if (result.rule == Styles::Disabled) {
            if (!doAssert && (seenEnabled == true)) return false;
            assert(seenEnabled == false);

            if (!doAssert && (result.score != 0)) return false;
            assert(result.score == 0);
        } else {
            if (i > 0) {
                const auto otherResult = this->results[i - 1];
                if (!doAssert && (result.rule == otherResult.rule)) return false;
                assert(result.rule != otherResult.rule);

                if (!doAssert && (otherResult.rule == Styles::Disabled)) return false;
                assert(otherResult.rule != Styles::Disabled);

                if (!doAssert && (otherResult.score < result.score)) return false;
                assert(otherResult.score >= result.score);
            }
        }
    }
    return true;
}

bool EvaluationSummary::operator==(const EvaluationSummary& other) {
    for (int i = 0; i < numStyles; ++i) {
        if (!(results[i] == other.results[i])) {
            return false;
        }
    }
    return true;
}

bool EvaluatorResult::operator==(const EvaluatorResult& other) {
    return score == other.score && rule == other.rule;
}
