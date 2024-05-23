#pragma once

#include "UIElement.h"

class QWidget;

VI_APPFWGUI_NS_BEGIN

class VI_APPFWGUI_API Control : public UIElement
{
    VI_OBJECT_META

protected:
    Control(QWidget *impl);

public:
    virtual ~Control();

public:
    Rect rect() const;
    Control *rect(const Rect &rect);

    Point position() const;
    Control *position(const Point &posi);

    Size size() const;
    Control *size(const Size &s);

private:
    VI_OBJECT_DATA
};

VI_APPFWGUI_NS_END