
#pragma once

#include "rack.hpp"
#include "plugin.hpp"

// This is an interface that clients need to implement.
class Dirty {
public:
    virtual bool isDirty() const = 0;
};

template <class T>
class BufferingParent : public Widget {
public:
    ~BufferingParent() {
        assert(_ownsChild);
      //  --_refCount;
    }
    /**
     * @brief Construct a new Buffering Parent object
     *
     * @param childWidget is the widget that will do all the work
     * @param size the size at which to create/set
     * @param dd usually will be childWidget, and childWidget will implement Dirty
     */
    BufferingParent(T *childWidget, const Vec size, Dirty *dd) {
        this->box.size = size;
        childWidget->box.size = size;
        _theWrappedChild = childWidget;
        _frameBufferWidget = new FramebufferWidget();
        this->addChild(_frameBufferWidget);
        _frameBufferWidget->addChild(childWidget);
        _dirtyDetector = dd;
      //  ++_refCount;
    }

    T *getChild() {
        return _theWrappedChild;
    }

    void step() override {
        Widget::step();
        if (_dirtyDetector && _dirtyDetector->isDirty()) {
            _frameBufferWidget->dirty = true;
        }
    }

    void setDirty() {
        _frameBufferWidget->dirty = true;
    }

private:
 //   static int _refCount;
    bool _ownsChild = true;
    T *_theWrappedChild;
    FramebufferWidget *_frameBufferWidget = nullptr;
    Dirty *_dirtyDetector = nullptr;
};
