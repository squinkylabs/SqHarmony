
#pragma once

#include <assert.h>

#include <algorithm>

#include "SqLog.h"

template <typename T, unsigned capacity>
class SqArray {
public:
    SqArray() {}
    SqArray(std::initializer_list<T> ilist) {
        _numValid = 0;
        _allowRandomAccess = false;
        for (auto iterator = ilist.begin(); iterator != ilist.end(); ++iterator) {
            _data[_numValid++] = *iterator;
        }
    }
    SqArray(const T* begin, const T* end) {
        _numValid = 0;
        _allowRandomAccess = false;
        for (auto iterator = begin; iterator != end; ++iterator) {
            _data[_numValid++] = *iterator;
        }
    }

    unsigned numValid() const {
        return _numValid;
    }

    const T& getAt(unsigned i) const {
        assert(i < (_allowRandomAccess ? capacity : _numValid));
        return _data[i];
    }

    void putAt(unsigned i, const T& data) {
        assert(i <= (_allowRandomAccess ? capacity : _numValid));  // only allow put at end, or overwrite old one
        _data[i] = data;
        _numValid = std::max(i + 1, _numValid);
    }

    T* getDirectPtrAt(unsigned int i) {
        assert(i <= (_allowRandomAccess ? capacity : _numValid));  // allowed to go one past, for std iterator conventions
        return _data + i;
    }

    const T* getDirectPtrAt(unsigned int i) const {
        assert(i <= (_allowRandomAccess ? capacity : _numValid));  // allowed to go one past, for std iterator conventions
        return _data + i;
    }

    void allowRandomAccess() {
        _allowRandomAccess = true;
    }

    void clear() {
        _numValid = 0;
    }

private:
    T _data[capacity];
    unsigned _numValid = 0;
    bool _allowRandomAccess = false;
};
