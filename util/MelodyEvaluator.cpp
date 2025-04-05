
#include "MelodyEvaluator.h"
#include "MelodyGenerator.h"

#include "NoteConvert.h"
#include "FloatNote.h"

float MelodyEvaluator::getPenalty(const MelodyRow& r, const MelodyMutateStyle& style) {
    float totalPenalty = 0;
    totalPenalty += leapsPenalty(r);
    totalPenalty += unisonsPenalty(r);
    totalPenalty += float((nonCenteredPenalty(r, style) * style.nonCenteredWeight));


    //SQINFO("returning penalty %f for row %s", totalPenalty, r.print().c_str());

    return totalPenalty;
}

float MelodyEvaluator::leapsPenalty(const MelodyRow& r) {
    int bigLeaps = 0;
    for (size_t i=0; i < r.getSize(); ++i) {
        const MidiNote& note1 = r.getNote(i);
        const MidiNote& note2 = r.getNote(i + 1);
        const int jump = std::abs( note1.get() - note2.get());
      
        if (jump > 4) {
            bigLeaps++;
        }
        //SQINFO("i=%d jump=%d big leaps=%d", i, jump, bigLeaps);
    }
    return float(bigLeaps) / float(r.getSize());
}

float MelodyEvaluator::unisonsPenalty(const MelodyRow& r) {
    int unisons = 0;
    for (size_t i=0; i < r.getSize(); ++i) {
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
    for (size_t i=0; i < r.getSize(); ++i) {
        const MidiNote& note = r.getNote(i);
        FloatNote floatNote;
        NoteConvert::m2f(floatNote, note);
      

       // totalDeviation += std::abs(note.get() - MidiNote::MiddleC);
        totalDeviation += std::abs(floatNote.get() - floatTarget.get());
       // SQINFO("in loop, note=%d total dev = %f", note.get(), totalDeviation);
    }

    const float penalty = totalDeviation / r.getSize();

  //  SQINFO("nonCenetered, PENALTY=%f total dev = %d", penalty, totalDeviation);

    // maj and minor centered with 1
    // and with .1
    // and with .01
    // and .0001
    // and .0000001
    // and .000000001
    // ng with  .00000000001
    // .0000000001 is good for now
    //assert(false);      // re-do this
  //  return penalty * .0000000001;
    return penalty;
};
