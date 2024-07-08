
#pragma once

#include <assert.h>

template <typename T, unsigned capacity>
class SqArray {
public:
    unsigned numValid() const {
        return _numValid;
    }
   
    const T& getAt(unsigned i) const {
        assert(i < _numValid);
        return _data[i];
    }

    void putAt(unsigned i, const T& data) {
        assert(i <= _numValid);      // only allow put at end, or overwrite old one
        _data[i] = data;
        _numValid = i+1;
    }

private:
    T _data[capacity];
    unsigned _numValid = 0;
};
