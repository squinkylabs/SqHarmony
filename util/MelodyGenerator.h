// #include <vector>
// #include <set>

#include "MidiNote.h"
#include "Scale.h"
#include "sq_rack.h"

class MelodyMutateStyle;

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

    std::string toString() const;
    MidiNote getAveragePitch() const;

    // Gets the index of the next note
    // static size_t nextNote(size_t index, size_t size);
    size_t wrapIndex(size_t index) const;

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

inline size_t MelodyRow::wrapIndex(size_t index) const {
    if (index >= getSize()) {
        index -= getSize();
    }
    assert(index < getSize());
    return index;
}

inline const MidiNote& MelodyRow::getNote(size_t index) const {
    assert(index <= _size);
    return (index == _size) ? notes[0] : notes[index];
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

class myxoro {
public:
    using result_type = uint64_t;
    void seed(result_type s1, result_type s2) { _random.seed(s1, s2); }
    result_type min() { return _random.min(); }
    result_type max() { return _random.max(); }
    result_type operator()() { return _random(); }

private:
    rack::random::Xoroshiro128Plus _random;
};

class BasicRandom {
public:
    BasicRandom(uint64_t seed1, uint64_t seed2) : _uniform(0, 1) {
        _random.seed(seed1, seed2);
    }
    // returns 0...1
    double generate() {
        const auto x = _uniform(_random);
        return x;
    }
    // generates random between 0..num-1
    int generateInteger(int num) {
        assert(num > 1);
        return std::round(generate() * (num-1));
    }

private:
    myxoro _random;
    std::uniform_real_distribution<double> _uniform;
};

class MelodyMutateState {
public:
    MelodyMutateState() : random(1234, 5678)  {
    }
    size_t nextToMutate = 0;
   // myxoro _generator;
   BasicRandom random;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
class MelodyGenerator {
public:
    static void mutate(MelodyRow& row, const Scale& scale, MelodyMutateState& state, const MelodyMutateStyle& style);
    static void getIndiciesToMutate(MelodyRow& row, MelodyMutateState& state, const MelodyMutateStyle& style, int* indiciesToMutate);

    static void _mutateOne(MelodyRow& row, size_t index, const Scale& scale, MelodyMutateState& state, const MelodyMutateStyle& style);
    static void _mutateSome(MelodyRow& row, const Scale& scale, MelodyMutateState& state, const MelodyMutateStyle& style, int* indiciesToMutate);
    static void _changeOneNoteInMode(MelodyRow& row, const Scale& scale, size_t index, int stepsToChange);

    static void makeStateLegal(MelodyMutateState& state, const MelodyRow& row);
    static bool toMutateIncludes(const int* indiciesToMutate, int candidateIndex);
};