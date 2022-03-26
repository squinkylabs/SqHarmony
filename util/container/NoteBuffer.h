#pragma once

#include "SqLog.h"
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

    void push_back(float cv1, float cv2, int channel);
    void removeForChannel(int channel);
    void removeAtIndex(int index);
    void setHold(bool);

    class Data {
    public:
        Data(float p1, float v1, int ch) : channel(ch), cv1(p1), cv2(v1) {}
        Data() {}
        int channel = 0;
        float cv1 = 0;
        float cv2 = 0;
    };

    const Data* begin() const;
    const Data* end() const;
    const Data& at(int index) const;

    using RejectFunction = std::function<bool(int index)>;

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

    Data data[maxCapacity + 2];
    void removeAll();
};

inline NoteBuffer::NoteBuffer(int cap) {
    assert(cap > 0);
    curCapacity = cap;
}

inline void NoteBuffer::setHold(bool h) {
    // SQINFO("set hold (%d) cur=%d", h, holdMode);
    if (holdMode == h) {
        return;
    }

    holdMode = h;
    if (!holdMode) {
        removeAll();
    }
}

inline void NoteBuffer::setCapacity(int size) {
    if (size == 0) {
        size = maxCapacity;
    }
    size = std::min(size, maxCapacity);
    if (size != curCapacity) {
        curCapacity = size;
      //  siz = std::min(siz, size);      // current size can't be more than we hold

        // if we are shrinking
        if (siz > size) {
            const int moveOffset = siz - size;
            assert(moveOffset > 0);
            for (int i = 0; i < size; ++i) {
                data[i] = data[i + moveOffset];
            }
            siz = size;
        }

        callbackMaybe();
    }
}

inline void NoteBuffer::onChange(callback callb) {
    cb = callb;
}

inline void NoteBuffer::push_back(float v1, float v2, int channel) {
    // SQINFO("nb push, siz=%d, cap=%d this=%p", siz, curCapacity, this);
    if (siz >= curCapacity) {
        for (int i = 0; i < siz - 1; ++i) {
            data[i] = data[i + 1];
        }
        data[siz - 1] = Data(v1, v2, channel);
    } else {
        data[siz] = Data(v1, v2, channel);
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
    // SQINFO("nb remove all");
    siz = 0;
    callbackMaybe();
}
