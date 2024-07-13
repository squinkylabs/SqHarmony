
#pragma once

#include <algorithm>
#include <assert.h>

#include "SqLog.h"

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
        _numValid = std::max(i+1, _numValid);
    }

    T* getDirectPtrAt(unsigned int i) {
         assert(i <= _numValid);    // allowed to go one past, for std iterator conventions 
        return _data + i;
    }

private:
    T _data[capacity];
    unsigned _numValid = 0;
};
