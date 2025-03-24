// #include <vector>
// #include <set>

#include "MidiNote.h"
#include "Scale.h"
#include "sq_rack.h"
// #include "rack.hpp"

class MelodyRow {
public:
    const static int maxNotes = 32;  // the longest our row can be
    /**
     * @brief initialize entire row to root of scale
     *
     * @param size
     * @param scale
     */
    void init(unsigned size, const Scale& scale);
    bool operator==(const MelodyRow& other) const;
    bool operator!=(const MelodyRow& other) const { return !(*this == other); }

    const MelodyRow& operator=(const MelodyRow& other);

    void setSize(size_t);
    size_t getSize() const { return _size; }

    const MidiNote& getNote(size_t index) const;
    void setNote(size_t index, const MidiNote& note);
    bool empty() const { return _size == 0; }

    std::string print() const;
    MidiNote getAveragePitch() const;

    // Gets the index of the next note
    static size_t nextNote(size_t index, size_t size);

private:
    MidiNote notes[maxNotes + 1];
    size_t _size = 0;
};

inline void MelodyRow::setSize(size_t size) {
    assert(size <= maxNotes);
    if (size <= maxNotes) {
        this->_size = size;
    }
}

inline const MidiNote& MelodyRow::getNote(size_t index) const {
    assert(index < _size);
    return notes[index];
}

inline void MelodyRow::setNote(size_t index, const MidiNote& note) {
    assert(index < _size);
    notes[index] = note;
}

inline const MelodyRow& MelodyRow::operator=(const MelodyRow& other) {
    this->_size = other._size;
    for (size_t i = 0; i < _size; ++i) {
        this->notes[i] = other.notes[i];
    }
    return (*this);
}

inline size_t MelodyRow::nextNote(size_t index, size_t size) {
    assert(size >= 0);
    return (index >= (size - 1)) ? 0 : index + 1;
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

inline bool MelodyRow::operator==(const MelodyRow& other) const {
    if (_size != other._size) {
        return false;
    }
    for (size_t i = 0; i < _size; ++i) {
        const MidiNote& a = getNote(i);
        const MidiNote& b = other.getNote(i);
        if (!(a == b)) {
            return false;
        }
    }
    return true;
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
    MelodyMutateState() {
        random.seed(1234, 5678);
    }
    size_t nextToMutate = 0;
    rack::random::Xoroshiro128Plus random;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
class MelodyGenerator {
public:
    static void mutate(MelodyRow& row, const Scale& scale, MelodyMutateState& state, MelodyMutateStyle& style);
    static void _changeOneNoteInMode(MelodyRow& row, const Scale& scale, size_t index, int stepsToChange);
};