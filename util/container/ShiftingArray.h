#pragma once

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
    void shift();

    const T& operator[](unsigned index) const;

    // using iterator = const std::vector<T>::iterator;

    //  typedef std::vector<BAR>::iterator bar_iterator;
    // LegacyRandomAccessIterator
    //  typedef std::vector<T>::iterator iteratorxx;

    // iterator begin();
    //  std::vector<T>::iterator begin();
    //   iterator end();
    using container = std::vector<T>;
    using iterator = typename container::iterator;

    iterator begin();
    iterator end();

private:
    container _vector;
};

template <typename T>
inline bool ShiftingArray<T>::empty() const {
    return _vector.empty();
}

template <typename T>
inline unsigned ShiftingArray<T>::size() const {
    return _vector.size();
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