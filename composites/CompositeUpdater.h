#pragma once

#include <vector>

template <typename T>
class ParamUpdater {
public:
    ParamUpdater(enum T::ParamIds paramID) : _paramID(paramID) {}
    ParamUpdater() = delete;
    bool poll(T* composite) const {
        const float f= composite->params[_paramID].value;
        const bool change = f != _lastTime;
        _lastTime = f;
        return change;
    }

private:
  //  T* const _composite;
    const int _paramID;
    mutable float _lastTime = -1000;
};

template <typename T>
class CVUpdater {
public:
    CVUpdater(enum T::InputIds inputID) : _inputID(inputID) {}
    CVUpdater() = delete;

    bool poll(T* composite) const {
        auto input = composite->inputs[_inputID];
        if (input.channels != _lastChannels) {
            _lastChannels = input.channels;
            return true;
        }
        for (int i=0; i < input.channels; ++i) {
            if (input.getVoltage(i) != _lastValues[i]) {
                 _lastValues[i] = input.getVoltage(i);
                 return true;
            }
        }
        return false;
    }

private:
 //   T* const _composite;
    const int _inputID;
    mutable float _lastValues[16] = {-1000};
    mutable int _lastChannels = -1;
};

template <typename T>
class CompositeUpdater {
public:
    CompositeUpdater(std::vector<ParamUpdater<T>>&&, std::vector<CVUpdater<T>>&&);
    bool poll() const;
private:
    std::vector<ParamUpdater<T>> _paramUpdaters;
    std::vector<CVUpdater<T>> _cvUpdaters;
    T* _composite = nullptr;
};

template <typename T>
inline CompositeUpdater<T>::CompositeUpdater(
    std::vector<ParamUpdater<T>>&& pu,
    std::vector<CVUpdater<T>>&& cu) : _cvUpdaters(std::move(cu)) {
}

template <typename T>
inline bool CompositeUpdater<T>::poll() const {
    assert(_composite);
    bool changed = false;
    for (auto x : _paramUpdaters) {
        changed |= x.poll();
    }
    for (auto x : _cvUpdaters) {
        changed |= x.poll();
    }
    return changed;
}
