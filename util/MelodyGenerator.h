#include <vector>
#include <set>

#include "MidiNote.h"
#include "Scale.h"

class MelodyRow {
public:
    /**
     * @brief initialize entire row to root of scale
     *
     * @param size
     * @param scale
     */
    void init(unsigned size, const Scale& scale);
    bool operator==(const MelodyRow& other) const;
    bool operator!=(const MelodyRow& other) const { return !(*this == other); }

    void setSize(unsigned);
    size_t getSize() const { return notes.size(); }

    MidiNote& getNote(size_t index) { return notes[index]; }
    std::string print() const;

    static size_t nextNote(size_t index, size_t size);
private:
    std::vector<MidiNote> notes;
};

inline size_t MelodyRow::nextNote(size_t index, size_t size) {
    assert(size >= 0);
    return (index >= (size -1)) ? 0 : index+1;
}

inline void MelodyRow::init(unsigned size, const Scale& scale) {
    setSize(size);
    ScaleNote scaleNote(0, 4);           // root, middle octave
    assert(scaleNote.getOctave() == 4);  // we want to init in middle octave.
    const MidiNote root = scale.s2m(scaleNote);
    for (unsigned i = 0; i < size; ++i) {
        notes[i] = root;
    }
}

inline void MelodyRow::setSize(unsigned size) {
    notes.clear();
    notes.resize(size);
}

inline bool MelodyRow::operator==(const MelodyRow& other) const {
    return notes == other.notes;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

class MelodyMutateStyle {
public:
    bool keepInScale = true;
    bool roundRobin = true;
    Scale scale;

};

class MelodyMutateState {
public:
    MelodyMutateState() {}
   // std::set<size_t> alreadyMutated;
   size_t nextToMutate = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
class MelodyGenerator {
public:
   
    static void mutate(MelodyRow& row, const Scale& scale, MelodyMutateState& state, MelodyMutateStyle& style);
private:
    static void changeOneNoteInMode(MelodyRow& row, const Scale& scale, size_t index, int stepsToChange);

};