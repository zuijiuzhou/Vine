#pragma once

#include "UIElement.hpp"

#include "Gui.hpp"

class QWidget;

V_APPFWGUI_NS_BEGIN

class V_APPFW_API Widget : public UIElement {
    V_OBJECT_META_DECL

  protected:
    Widget(QWidget* impl);

  public:
    virtual ~Widget();

  public:
    Rect rect() const;
    void rect(const Rect& rect);

    Point position() const;
    void  position(const Point& posi);

    Size size() const;
    void size(const Size& s);

  private:
    struct Data;
    Data* const d;
};

V_APPFWGUI_NS_END
