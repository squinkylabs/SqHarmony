#include <vector>

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
    void init(size_t size, const Scale& scale);
    bool operator==(const MelodyRow& other) const;
    bool operator!=(const MelodyRow& other) const { return !(*this == other); }


    void setSize(unsigned);
    size_t _getSize() const { return notes.size(); }

    const MidiNote& getNote(size_t index) { return notes[index]; }

private:
    std::vector<MidiNote> notes;
};

inline void MelodyRow::init(size_t size, const Scale& scale) {
    setSize(size);

    // get the root of the scale, normalize to middle octave
    // TODO: this gets relative!!!
   // const int semi = scale.degreeToSemitone(0);
 //   const MidiNote root(MidiNote::MiddleC + semi);

    ScaleNote scaleNote(0, 4);          // root, middle octave
    assert(scaleNote.getOctave() == 4);         // we want to init in middle octave.
    const MidiNote root = scale.s2m(scaleNote);
    for (int i = 0; i < size; ++i) {
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

class MelodyGenerator {
public:
    static void changeOneNote(MelodyRow& row, size_t index, int stepsToChange);
};