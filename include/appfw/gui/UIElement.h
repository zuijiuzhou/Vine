#pragma once

#include "gui_global.h"

#include "core/Object.h"
#include "core/Events.h"
#include "core/String.h"

VINE_APPFWGUI_NS_BEGIN

class VINE_APPFWGUI_API UIElement : public Object
{
    VI_OBJECT_META
public:
    UIElement();

private:
    VI_DISABLE_COPY_MOVE(UIElement);

public:
    String name() const;
    virtual UIElement *name(const String &name);

public:
    const Event<UIElement, PropertyChangedEventArgs<String>> NameChanged;

private:
    VI_OBJECT_DATA
};

VINE_APPFWGUI_NS_END