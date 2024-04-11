#pragma once

#include <assert.h>

#include <functional>
#include <vector>

#include "SqLog.h"

using CVMapFunction = std::function<int(float)>;

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

enum class PolyMono {
    Poly,
    Mono
};

template <typename T>
class CVInUpdater {
public:
    CVInUpdater(
        enum T::InputIds inputID,
        PolyMono polyMono,
        CVMapFunction mapFunction) : _inputID(inputID),
                                     _inputIsMonophonic(polyMono == PolyMono::Mono),
                                     _cvMapFunction(mapFunction) {
    }
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
            const float in = input.getVoltage(i); 
            if (in != _lastValues[i]) {
                _lastValues[i] = in;
                if (_cvMapFunction) {
                    if (_cvMapFunction(in) != _lastMappedValues[i]) {
                     _lastMappedValues[i] = _cvMapFunction(in);
                     return true;
                    }
                } else {
                    return true;
                } 
            }
        }
        return false;
    }

private:
    const int _inputID;
    const bool _inputIsMonophonic;
    const CVMapFunction _cvMapFunction;
    mutable float _lastValues[16] = {-1000};
    mutable int _lastMappedValues[16] = {-1000};
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

    void add(enum T::ParamIds param, bool everyPoll = true) {
        assert(_composite);
        auto& list = everyPoll ? _paramUpdaters : _paramUpdatersInfrequent;
        list.push_back({param});
    }

    void add(enum T::InputIds input, PolyMono monoPoly, bool everyPoll, CVMapFunction mapFunction) {
        assert(_composite);
        auto& list = everyPoll ? _cvInUpdaters : _cvInUpdatersInfrequent;
        list.push_back({input, monoPoly, mapFunction});
    }

    void add(enum T::OutputIds output, bool everyPoll) {
        assert(_composite);
        auto& list = everyPoll ? _cvOutUpdaters : _cvOutUpdatersInfrequent;
        list.push_back({output});
    }

    void set(T* composite, int infrequentMult) {
        assert(!_composite);
        assert(composite);
        _composite = composite;
        _subdivision = infrequentMult;
    }

private:
    std::vector<ParamUpdater<T>> _paramUpdaters;
    std::vector<ParamUpdater<T>> _paramUpdatersInfrequent;
    std::vector<CVInUpdater<T>> _cvInUpdaters;
    std::vector<CVInUpdater<T>> _cvInUpdatersInfrequent;
    std::vector<CVOutUpdater<T>> _cvOutUpdaters;
    std::vector<CVOutUpdater<T>> _cvOutUpdatersInfrequent;
    const T* _composite = nullptr;
    int _subdivision = 1;
    bool _firstTime = true;
    mutable int _counter = 0;

    bool pollFrequent() const;
    bool pollInFrequent() const;
};

template <typename T>
inline bool CompositeUpdater<T>::poll() const {
    bool changed = pollFrequent();
    if (_counter <= 0) {
        changed |= pollInFrequent();
        _counter = _subdivision;
    }
    --_counter;
    return changed;
}

template <typename T>
inline bool CompositeUpdater<T>::pollInFrequent() const {
    assert(_composite);
    bool changed = false;
    for (int i = 0; i < int(_paramUpdatersInfrequent.size()); ++i) {
        changed |= _paramUpdatersInfrequent[i].poll(_composite);
    }
    for (int i = 0; i < int(_cvInUpdatersInfrequent.size()); ++i) {
        changed |= _cvInUpdatersInfrequent[i].poll(_composite);
    }

    for (int i = 0; i < int(_cvOutUpdatersInfrequent.size()); ++i) {
        changed |= _cvOutUpdatersInfrequent[i].poll(_composite);
    }

    return changed;
}

template <typename T>
inline bool CompositeUpdater<T>::pollFrequent() const {
    assert(_composite);
    bool changed = false;
    for (int i = 0; i < int(_paramUpdaters.size()); ++i) {
        changed |= _paramUpdaters[i].poll(_composite);
    }

    for (int i = 0; i < int(_cvInUpdaters.size()); ++i) {
        //      SQINFO("polling freq cvin");
        changed |= _cvInUpdaters[i].poll(_composite);
        //    SQINFO("done polling freq cvin");
    }

    for (int i = 0; i < int(_cvOutUpdaters.size()); ++i) {
        changed |= _cvOutUpdaters[i].poll(_composite);
    }
    //   assert(false);
    return changed;
}
