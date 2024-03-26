#pragma once

#include <vector>

template <typename T>
class ParamUpdater {
public:
    ParamUpdater(T* composite, int paramID) : _composite(composite), _paramID(paramID) {}
    ParamUpdater() = delete;

private:
    T* const _composite;
    const int _paramID;
};

template <typename T>
class CVUpdater {
public:
    CVUpdater(T* composite, int inputID) : _composite(composite), _inputID(inputID) { }

   CVUpdater() = delete;
  //  CVUpdater(const CVUpdater&) = delete;

private:
    T* const _composite;
    const int _inputID;
};

template <typename T>
class CompositeUpdater {
public:
    CompositeUpdater(std::vector<ParamUpdater<T>>&&, std::vector<CVUpdater<T>>&& );
private:
    std::vector<ParamUpdater<T>> _paramUpdaters;
    std::vector<CVUpdater<T>> _cvUpdaters;
};

template <typename T>
inline CompositeUpdater<T>::CompositeUpdater(
    std::vector<ParamUpdater<T>>&& pu, 
    std::vector<CVUpdater<T>>&& cu) : _cvUpdaters(std::move(cu)) {
}
