#pragma once

#include <assert.h>

#include <algorithm>
#include <vector>

/**
 * @brief Thin wrapper around std::vector
 *
 */
template <typename T>
class ShiftingArray {
public:
    bool empty() const;
    unsigned size() const;
    void push_back(const T& t);
    void shift(const T& t);  // pushes 't' in the front, if anything in array.
    void shift(unsigned times, const T& t);

    const T& operator[](unsigned index) const;

    using container = std::vector<T>;
    using iterator = typename container::iterator;

    iterator begin();
    iterator end();

private:
    container _vector;
};

template <typename T>
inline void ShiftingArray<T>::shift(const T& t) {
    shift(1, t);
}

template <typename T>
inline void ShiftingArray<T>::shift(unsigned times, const T& t) {
    if (_vector.empty()) {
        return;
    }
    for (unsigned i = 0; i < times; ++i) {
        const auto iterator = _vector.begin();
        assert(iterator != _vector.end());
        _vector.insert(iterator, t);
    }
}

template <typename T>
inline bool ShiftingArray<T>::empty() const {
    return _vector.empty();
}

template <typename T>
inline unsigned ShiftingArray<T>::size() const {
    return unsigned(_vector.size());
}

template <typename T>
inline void ShiftingArray<T>::push_back(const T& t) {
    _vector.push_back(t);
}

template <typename T>
inline const T& ShiftingArray<T>::operator[](unsigned index) const {
    return _vector[index];
}

template <typename T>
typename ShiftingArray<T>::iterator ShiftingArray<T>::begin() {
    return _vector.begin();
}

template <typename T>
typename ShiftingArray<T>::iterator ShiftingArray<T>::end() {
    return _vector.end();
}