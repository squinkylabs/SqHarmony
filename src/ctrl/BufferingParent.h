
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
        INFO("deleting BufferingParent %p", this);
        assert(_ownsChild);
    }
    /**
     * @brief Construct a new Buffering Parent object
     *
     * @param childWidget is the widget that will do all the work
     * @param size the size at which to create/set
     * @param dd usually will be childWidget, and childWidget will implement Dirty
     */
    BufferingParent(Widget* childWidget, const Vec size, Dirty *dd) {
        INFO("buffering parent ctor with size=%f, %f", size.x, size.y);
        this->box.size = size;
        childWidget->box.size = size;
        _theWrappedChild = childWidget;
        _frameBufferWidget = new FramebufferWidget();
        this->addChild(_frameBufferWidget);
        _frameBufferWidget->addChild(childWidget);
        _dirtyDetector = dd;

         _setHackParent(childWidget);

         INFO("fw pos=%f, %f size=%f, %f", _frameBufferWidget->box.pos.x, _frameBufferWidget->box.pos.y, _frameBufferWidget->box.size.x, _frameBufferWidget->box.size.y);


        INFO("----- ctor pos=%f, %f size=%f, %f", box.pos.x, box.pos.y, box.size.x, box.size.y);
       // INFO("----- ctor  hack p: pos=%f, %f size=%f, %f", _hackParent->box.pos.x,  _hackParent->box.pos.y,_hackParent->box.size.x,  _hackParent->box.size.y);

    }

    void _setHackParent(Widget *childWidget);

    void step() override {
     //   SQINFO("BufferingParent::step. this=%p", this);
        Widget::step();
        if (_dirtyDetector && _dirtyDetector->isDirty()) {
            _frameBufferWidget->dirty = true;
            SQINFO("BP set dirty true 46");
        }
    }

private:
    bool _ownsChild = true;
    Widget* _theWrappedChild;
    FramebufferWidget *_frameBufferWidget = nullptr;
    Dirty *_dirtyDetector = nullptr;
};
