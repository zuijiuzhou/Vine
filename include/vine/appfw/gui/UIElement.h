#pragma once

#include <vine/core/Events.h>
#include <vine/core/Object.h>
#include <vine/core/String.h>
#include "gui_global.h"

#include "Gui.h"

class QObject;

VI_APPFWGUI_NS_BEGIN

class VI_APPFWGUI_API UIElement : public Object {
    VI_OBJECT_META

    VI_DISABLE_COPY_MOVE(UIElement)

  protected:
    UIElement(QObject* impl);

  public:
    virtual ~UIElement();

  public:
    String       name() const;
    virtual void name(const String& name);

  public:
    const Event<UIElement, PropertyChangedEventArgs<String>> NameChanged;

  //protected:
    virtual QObject* impl() const;

    template <typename TImpl> TImpl* impl() const { return (TImpl*)(impl()); }

  private:
    VI_OBJECT_DATA
};

VI_APPFWGUI_NS_END