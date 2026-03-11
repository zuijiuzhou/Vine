#pragma once

#include <vine/appfw/appfw_global.hpp>

#include <vine/Events.hpp>
#include <vine/RefObject.hpp>
#include <vine/String.hpp>

class QObject;

VI_APPFWGUI_NS_BEGIN
using UIObject = QObject;

class VI_APPFW_API UIElement : public RefObject {
    VI_OBJECT_META_DECL

  protected:
    UIElement(QObject* impl);

  public:
    virtual ~UIElement();

  public:
    String       name() const;
    virtual void name(const String& name);

  public:
    const Event<UIElement, PropertyChangedEventArgs<String>> NameChanged;

    // protected:
    virtual UIObject* impl() const;

    template <typename TImpl>
    TImpl* impl() const
    {
        return (TImpl*)impl();
    }

  private:
    VI_OBJECT_DATA
};

VI_APPFWGUI_NS_END
