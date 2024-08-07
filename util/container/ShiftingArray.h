#pragma once

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

    const T& operator [](unsigned index) const;

private:
    std::vector<T> _vector;
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
inline const T& ShiftingArray<T>::operator [](unsigned index) const {
   return _vector[index];
}