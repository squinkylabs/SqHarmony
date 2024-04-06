#pragma once

#include <assert.h>

#include <vector>

#include "SqLog.h"

template <typename T>
class ParamUpdater {
public:
    ParamUpdater(enum T::ParamIds paramID) : _paramID(paramID) {}
    ParamUpdater() = delete;
    bool poll(const T* composite) const {
        const float f = composite->params[_paramID].value;
        const bool change = (f != _lastTime);
        _lastTime = f;
        return change;
    }

private:
    const int _paramID;
    mutable float _lastTime = -1000;
};

template <typename T>
class CVInUpdater {
public:
    CVInUpdater(enum T::InputIds inputID, bool inputIsMonophonic) : _inputID(inputID),
                                                                  _inputIsMonophonic(inputIsMonophonic) {}
    CVInUpdater() = delete;

    bool poll(const T* composite) const {
        auto input = composite->inputs[_inputID];
        if (input.channels != _lastChannels) {
            _lastChannels = input.channels;
            _snapshotValues(composite);
            _lastChannels = input.channels;
            return true;
        }
        const unsigned maxChannels = _inputIsMonophonic ? 1 : 16;
        const unsigned channelToPoll = std::min(maxChannels, unsigned(input.channels));
        for (unsigned i = 0; i < channelToPoll; ++i) {
            if (input.getVoltage(i) != _lastValues[i]) {
                _lastValues[i] = input.getVoltage(i);
                return true;
            }
        }
        return false;
    }

private:
    const int _inputID;
    const bool _inputIsMonophonic;
    mutable float _lastValues[16] = {-1000};
    mutable int _lastChannels = -1;

    void _snapshotValues(const T* composite) const {
        auto input = composite->inputs[_inputID];
        for (int i = 0; i < input.channels; ++i) {
            _lastValues[i] = input.getVoltage(i);
        }
    }
};

// Monitors output port. Only to know is it got hooked up.
template <typename T>
class CVOutUpdater {
public:
    CVOutUpdater(enum T::OutputIds outputID) : _outputID(outputID) {}
    CVOutUpdater() = delete;
    bool poll(const T* composite) const {
        auto output = composite->outputs[_outputID];
        if (output.channels != _lastChannels) {
            _lastChannels = output.channels;
            return true;
        }
        return false;
    }

private:
    const int _outputID;
    mutable int _lastChannels = -1;
};

template <typename T>
class CompositeUpdater {
public:
    CompositeUpdater(T* composite) : _composite(composite) {}
    CompositeUpdater() {}
    CompositeUpdater(const CompositeUpdater&) = delete;
    bool poll() const;

    void add(enum T::ParamIds param) {
        assert(_composite);
        _paramUpdaters.push_back({param});
    }

    void add(enum T::InputIds input, bool isMonophonic) {
        assert(_composite);
        _cvInUpdaters.push_back({input, isMonophonic});
    }

    void add(enum T::OutputIds output) {
        assert(_composite);
        _cvOutUpdaters.push_back({output});
    }

    void set(T* composite) {
        assert(!_composite);
        assert(composite);
        _composite = composite;
    }

private:
    std::vector<ParamUpdater<T>> _paramUpdaters;
    std::vector<CVInUpdater<T>> _cvInUpdaters;
    std::vector<CVOutUpdater<T>> _cvOutUpdaters;
    const T* _composite = nullptr;
};

template <typename T>
inline bool CompositeUpdater<T>::poll() const {
    assert(_composite);
    bool changed = false;
    for (int i = 0; i < int(_paramUpdaters.size()); ++i) {
        changed |= _paramUpdaters[i].poll(_composite);
    }

    for (int i = 0; i < int(_cvInUpdaters.size()); ++i) {
        changed |= _cvInUpdaters[i].poll(_composite);
    }

    for (int i = 0; i < int(_cvOutUpdaters.size()); ++i) {
        changed |= _cvOutUpdaters[i].poll(_composite);
    }
    return changed;
}
