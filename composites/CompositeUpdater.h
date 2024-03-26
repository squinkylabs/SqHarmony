#pragma once

#include <vector>

template <typename T>
class ParamUpdater {
public:
    ParamUpdater(T* composite, int paramID);
    ParamUpdater() = delete;
    ParamUpdater(const ParamUpdater&) = delete;

private:
    T* const _composite;
};

template <typename T>
class CVUpdater {
public:
    CVUpdater(T* composite, int paramID);
    CVUpdater() = delete;
    CVUpdater(const CVUpdater&) = delete;

private:
    T* const _composite;
};

template <typename T>
class CompositeUpdater {
public:
    CompositeUpdater(const std::vector<ParamUpdater<T>>&, const std::vector<CVUpdater<T>>& );
private:
    std::vector<ParamUpdater<T>> _paramUpdaters;
    std::vector<CVUpdater<T>> _cvUpdaters;
};
