#pragma once

#include <vector>

template <typename T>
class ParamUpdater {
public:
    ParamUpdater(enum T::ParamIds paramID) : _paramID(paramID) {}
    ParamUpdater() = delete;
    bool poll(const T* composite) const {
        const float f = composite->params[_paramID].value;
        const bool change = f != _lastTime;
        _lastTime = f;
        return change;
    }

private:
    const int _paramID;
    mutable float _lastTime = -1000;
};

template <typename T>
class CVUpdater {
public:
    CVUpdater(enum T::InputIds inputID) : _inputID(inputID) {}
    CVUpdater() = delete;

    bool poll(const T* composite) const {
        auto input = composite->inputs[_inputID];
        if (input.channels != _lastChannels) {
            _lastChannels = input.channels;
            _snapshotValues(composite);
            return true;
        }
        for (int i = 0; i < input.channels; ++i) {
            if (input.getVoltage(i) != _lastValues[i]) {
                _lastValues[i] = input.getVoltage(i);
                return true;
            }
        }
        return false;
    }

private:
    const int _inputID;
    mutable float _lastValues[16] = {-1000};
    mutable int _lastChannels = -1;

    void _snapshotValues(const T* composite) const {
        auto input = composite->inputs[_inputID];
        for (int i = 0; i < input.channels; ++i) {
            _lastValues[i] = input.getVoltage(i);
        }
    }
};

template <typename T>
class CompositeUpdater {
public:
    CompositeUpdater(T* composite) : _composite(composite) {}
    CompositeUpdater() = delete;
    CompositeUpdater(const CompositeUpdater&) = delete;
    bool poll() const;
    void add(enum T::ParamIds param) {
        _paramUpdaters.push_back({ param });
    }
    void add(enum T::InputIds input) {
        _cvUpdaters.push_back({input});
    }

private:
    std::vector<ParamUpdater<T>> _paramUpdaters;
    std::vector<CVUpdater<T>> _cvUpdaters;
    const T* _composite = nullptr;
};

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
