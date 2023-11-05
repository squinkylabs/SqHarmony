
#pragma once

#include "rack.hpp"

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
    }

    T *getChild() {
        return _theWrappedChild;
    }

    void step() override {
        //   SQINFO("BufferingParent::step. this=%p", this);
        Widget::step();
        if (_dirtyDetector && _dirtyDetector->isDirty()) {
            _frameBufferWidget->dirty = true;
            // SQINFO("BP set dirty true 46");
        }
    }

private:
    bool _ownsChild = true;
    T *_theWrappedChild;
    FramebufferWidget *_frameBufferWidget = nullptr;
    Dirty *_dirtyDetector = nullptr;
};
