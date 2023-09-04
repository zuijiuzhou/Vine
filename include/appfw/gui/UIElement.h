#pragma once

#include "gui_global.h"

#include "core/Object.h"
#include "core/Events.h"
#include "core/String.h"

VINE_APPFWGUI_NS_BEGIN

class VINE_APPFWGUI_API UIElement : public Object
{
    VI_OBJECT_META
protected:
    UIElement(void* impl);

private:
    VI_DISABLE_COPY_MOVE(UIElement);

public:
    String name() const;
    virtual UIElement *name(const String &name);

public:
    const Event<UIElement, PropertyChangedEventArgs<String>> NameChanged;

protected:
    virtual void* impl() const;

    template<typename TImpl>
    TImpl* impl() const{
        return static_cast<TImpl*>(impl());
    }

private:
    VI_OBJECT_DATA
};

VINE_APPFWGUI_NS_END