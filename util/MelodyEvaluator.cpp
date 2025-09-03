
#include "MelodyEvaluator.h"

#include "FloatNote.h"
#include "MelodyGenerator.h"
#include "NoteConvert.h"

std::string MelodyMutateStyle::toString() const {
    std::stringstream s;
    s << "non ctr " << this->nonCenteredWeight;
    s << " range " << this->pitchRangeWeight;
    s << " leaps " << this->leapsWeight;
    s << " unis: " << this->unisonWeight;
    s << " center-v=" << this->centerVoltage;
    s << std::endl;
    return s.str();
}

void MelodyMutateStyle::setStyles(Styles theStyle) {
    // first, disable everything
    nonCenteredWeight = 0;
    pitchRangeWeight = 0;
    leapsWeight = 0;
    unisonWeight = 0;
    dissonantWeight = 0;
    switch (theStyle) {
        case Styles::OnlySeekCenter:
            nonCenteredWeight = 1;
            break;
        case Styles::Disabled:
            break;
        case Styles::OnlyDissonant:
            dissonantWeight = 1;
            break;
        default:
            assert(false);
    }
}

float MelodyEvaluator::getPenalty(const MelodyRow& r, const MelodyMutateStyle& style) {
    const float lp = leapsPenalty(r, style);
    const float up = unisonsPenalty(r, style);
    const float cp = nonCenteredPenalty(r, style);
    const float prp = pitchRangePenalty(r, style);
    const float disp = disonnantPenalty(r, style);

    const float total = lp + up + cp + prp + disp;
    return total;
}

std::string MelodyEvaluator::toString(const MelodyRow& row, const MelodyMutateStyle& style) {
    std::stringstream s;
    s << row.toString();
    s << " penalty=";
    s << getPenalty(row, style);
    s << " leap=" << leapsPenalty(row, style);
    s << " uni=" << unisonsPenalty(row, style);
    s << " non-cent=" << float((nonCenteredPenalty(row, style) * style.nonCenteredWeight));
    s << " range=" << pitchRangePenalty(row, style);
    s << " disson=" << disonnantPenalty(row, style);
    // SQINFO("weights = %f, %f, %f, %f", style.nonCenteredWeight, style.pitchRangeWeight, style.unisonWeight, style.leapsWeight);

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
    return style.leapsWeight * float(bigLeaps) / float(r.getSize());
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
    // SQINFO("unisons = %d, size=%lld", unisons, r.getSize());
    return style.unisonWeight * (unisons) / float(r.getSize());
}

float MelodyEvaluator::nonCenteredPenalty(const MelodyRow& r, const MelodyMutateStyle& style) {
    // SQINFO("enter nonCenteredPenalty, target vs=%f", style.centerVoltage);
    assert(r.getSize() > 0);
    float totalDeviation = 0;
    FloatNote floatTarget(style.centerVoltage);
    for (size_t i = 0; i < r.getSize(); ++i) {
        const MidiNote& note = r.getNote(i);
        FloatNote floatNote;
        NoteConvert::m2f(floatNote, note);

        // totalDeviation += std::abs(note.get() - MidiNote::MiddleC);
        totalDeviation += std::abs(floatNote.get() - floatTarget.get());
        // SQINFO("in loop, i=%d note=%d,%f total dev = %f", i, note.get(), floatNote.get(), totalDeviation);
    }

    const float penalty = totalDeviation / r.getSize();

    // results of tuning to make long term drift pass
    // for a long time was mult by .001 * .01 * .04 . Now with new probability stuff drift tests are failing
    // double k = .000001;  // 10 too high
    // passes at .000001
    //  .0000001 too low

  //  double k = .00001;  // .00001 worked ok
    double k = .1;



   // SQINFO("final penalty = %f", penalty * style.nonCenteredWeight * k);
    return penalty * style.nonCenteredWeight * k;
};


float MelodyEvaluator::disonnantPenalty(const MelodyRow& r, const MelodyMutateStyle& style) {
    const Scale& scale = style.scale;
    
    const int basePitch = scale.base().get() % 12;
    const int fifthPitch = (basePitch + 7) % 12;
    const int fourthPitch = (basePitch + 5) % 12;
   // SQINFO("base pitch=%d fifth=%d fourth%d", basePitch, fifthPitch, fourthPitch);
  //  SQINFO(" scale base=%d scale=%d wasSet=%d", scale.base().get() , scale.get().first.get(), scale.getWasSet());
    
  //  bool gotBase = false;
    int bases = 0;
    int disonnances = 0;
    for (size_t i = 0; i < r.getSize(); ++i) {
        const MidiNote& note = r.getNote(i);
        const int notePitch = note.get() % 12;
      //  SQINFO("note pitch = %d", notePitch);

        if (notePitch == basePitch) {
           // SQINFO("!!! Match !!!");
           bases++;
        }
        if (
            (notePitch != basePitch) &&
            (notePitch != fifthPitch) &&
            (notePitch != fourthPitch)
            ) {
            ++disonnances;
        }
    }
    // if (bases > 1) {
    //     SQINFO("got base, row = %s diss=%d bases=%d", r.toString().c_str(), disonnances, bases);
    // }
    const float ret =  style.dissonantWeight * float(disonnances - bases) / float(r.getSize());
    return std::max(ret, 0.f);
}

float MelodyEvaluator::pitchRangePenalty(const MelodyRow& r, const MelodyMutateStyle& style) {
    int min = 200;
    int max = -200;
    for (size_t i = 0; i < r.getSize(); ++i) {
        const MidiNote& note = r.getNote(i);
        min = std::min(min, note.get());
        max = std::max(max, note.get());
    }
    const int range = max - min;
    const int diff = std::abs(style.idealPitchRange2 - range);
    const float ret = (diff / 12.) * style.pitchRangeWeight;
    return ret;
};
