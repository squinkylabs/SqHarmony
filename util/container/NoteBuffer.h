#pragma once

#include <assert.h>
#include <functional>

class NoteBuffer {
public:
    NoteBuffer(int cap);
    void setCapacity(int);
    using callback = std::function<void(const NoteBuffer*)>;
    void onChange(callback);

    int getCapacity() const {
        return curCapacity;
    }
    static const int maxCapacity{32};
    int size() const { return siz; }
    bool empty() const { return siz == 0; }

    void push_back(float pitch, int channel);
    //  void remove(float);
    void removeForChannel(int channel);
    void removeAtIndex(int index);
    void setHold(bool);

    class Data {
    public:
        Data(float p, int ch) : channel(ch), cv(p) {}
        Data() {}
        int channel = 0;
        float cv = 0;
    };

    const Data* begin() const;
    const Data* end() const;
    const Data& at(int index) const;

    using RejectFunction = std::function<bool(int index)>;
   // int findMedian(RejectFunction = nullptr);

private:
    int siz = 0;
    int curCapacity = 1;
    bool holdMode = false;
    callback cb;

    void callbackMaybe() {
        if (cb) {
            cb(this);
        }
    }

    Data data[maxCapacity];
    void removeAll();
};

inline NoteBuffer::NoteBuffer(int cap) {
    assert(cap > 0);
    curCapacity = cap;
}

inline void NoteBuffer::setHold(bool h) {
    if (holdMode == h) {
        return;
    }

    holdMode = h;
    if (!holdMode) {
        removeAll();
    }
}

inline void NoteBuffer::setCapacity(int size) {
    size = std::min(size, maxCapacity);
    if (size != curCapacity) {
        curCapacity = size;
        callbackMaybe();
    }
}

inline void NoteBuffer::onChange(callback callb) {
    cb = callb;
}

inline void NoteBuffer::push_back(float x, int channel) {
    if (siz == curCapacity) {
        for (int i = 0; i < siz - 1; ++i) {
            data[i] = data[i + 1];
        }
        data[siz - 1] = Data(x, channel);
    } else {
        data[siz] = Data(x, channel);
        siz++;
    }
    callbackMaybe();
}

inline const NoteBuffer::Data* NoteBuffer::begin() const {
    return data;
}

inline const NoteBuffer::Data* NoteBuffer::end() const {
    return data + size();
}

inline  const NoteBuffer::Data& NoteBuffer::at(int index) const {
    assert(index < siz);
    return data[index];
}

inline void NoteBuffer::removeForChannel(int channel) {
    // printf("remove for channel called ch=%d siz=%d\n", channel, siz);
    if (holdMode) {
        // printf("remove does nothing, hold on\n");
        return;
    }
    for (int i = 0; i < siz; ++i) {
        // printf("remove i=%d, data.ch=%d, p=%f\n", i, data[i].channel, data[i].cv);
        if (channel == data[i].channel) {           
            return removeAtIndex(i);
        }
    }
}

inline void NoteBuffer::removeAtIndex(int index) {
    for (int i = index; i < (siz - 1); ++i) {
        data[i] = data[i + 1];
    }
    if (siz) {
        --siz;
    }
    callbackMaybe();
}

inline void NoteBuffer::removeAll() {
    siz = 0;
}

// TODO: get rid of this
#if 0
 inline int NoteBuffer::findMedian(RejectFunction rejectFun) {
    float upperBound = 100;
    float lowerBound = -100;
    int bestIndex = -1;

    for (int index = 0; index < siz; ++index) {
        const float x = data[index].cv;
        if (x < upperBound) {
            upperBound = x;
            bestIndex = index;
        } else if (x > lowerBound) {
            lowerBound = x;
            bestIndex = index;
        }
    }
    return bestIndex;
 }
 #endif