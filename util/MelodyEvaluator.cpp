
#include "MelodyEvaluator.h"

#include "FloatNote.h"
#include "MelodyGenerator.h"
#include "NoteConvert.h"

std::string MelodyMutateStyle::toString() const {
    std::stringstream s;
    s << "non ctr " << this->nonCenteredWeight;
    s << " range " << this->pitchRangeWeight;
    s << " leaps " << this->leapsWeight;
    s << " unis: " << this->unisonWeight << std::endl;
    return s.str();
}

float MelodyEvaluator::getPenalty(const MelodyRow& r, const MelodyMutateStyle& style) {
    float totalPenalty = 0;
    totalPenalty += leapsPenalty(r, style);
    totalPenalty += unisonsPenalty(r, style);
    totalPenalty += nonCenteredPenalty(r, style);
    totalPenalty += pitchRangePenalty(r, style);
    return totalPenalty;
}

std::string MelodyEvaluator::toString(const MelodyRow& row, const MelodyMutateStyle& style) {
    std::stringstream s;
    s << row.toString();
    s << " penalty =";
    s << getPenalty(row, style);
    s << " leap: " << leapsPenalty(row, style);
    s << " uni: " << unisonsPenalty(row, style);
    s << " nonc: " << float((nonCenteredPenalty(row, style) * style.nonCenteredWeight));
    s << " range: " << pitchRangePenalty(row, style);

    return s.str();
}

float MelodyEvaluator::leapsPenalty(const MelodyRow& r, const MelodyMutateStyle& style) {
    int bigLeaps = 0;
    for (size_t i = 0; i < r.getSize(); ++i) {
        const MidiNote& note1 = r.getNote(i);
        const MidiNote& note2 = r.getNote(i + 1);
        const int jump = std::abs(note1.get() - note2.get());

        if (jump > 4) {
            bigLeaps++;
        }
        // SQINFO("i=%d jump=%d big leaps=%d", i, jump, bigLeaps);
    }
    return float(bigLeaps) / float(r.getSize());
}

float MelodyEvaluator::unisonsPenalty(const MelodyRow& r, const MelodyMutateStyle& style) {
    int unisons = 0;
    for (size_t i = 0; i < r.getSize(); ++i) {
        const MidiNote& note1 = r.getNote(i);
        const MidiNote& note2 = r.getNote(i + 1);
        if (note1.get() == note2.get()) {
            unisons++;
        }
    }

    // a single unison doesn't count.
    if (unisons <= 1) {
        return 0;
    }
    return float(unisons) / float(r.getSize());
}

float MelodyEvaluator::nonCenteredPenalty(const MelodyRow& r, const MelodyMutateStyle& style) {
    // SQINFO("enter eval non cent, row=%s", r.print().c_str());
    assert(r.getSize() > 0);
    float totalDeviation = 0;
    FloatNote floatTarget(style.centerVoltage);
    for (size_t i = 0; i < r.getSize(); ++i) {
        const MidiNote& note = r.getNote(i);
        FloatNote floatNote;
        NoteConvert::m2f(floatNote, note);

        // totalDeviation += std::abs(note.get() - MidiNote::MiddleC);
        totalDeviation += std::abs(floatNote.get() - floatTarget.get());
        // SQINFO("in loop, note=%d total dev = %f", note.get(), totalDeviation);
    }

    const float penalty = totalDeviation / r.getSize();
    return penalty * style.nonCenteredWeight * .001 * .01 * .04;
};

float MelodyEvaluator::pitchRangePenalty(const MelodyRow& r, const MelodyMutateStyle& style) {
    int min = 200;
    int max = -200;
    for (size_t i = 0; i < r.getSize(); ++i) {
        const MidiNote& note = r.getNote(i);
        min = std::min(min, note.get());
        max = std::max(max, note.get());
    }
    const int range = max - min;
    // SQINFO("range=%d min=%d max=%d", range, min, max);

    const int diff = std::abs(style.idealPitchRange - range);

    return (diff / 12.) * style.pitchRangeWeight;
};
