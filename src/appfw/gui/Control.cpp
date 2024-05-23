#include <appfw/gui/Control.h>
#include <QWidget>

#include "Convert.h"

VI_APPFWGUI_NS_BEGIN

VI_OBJECT_META_IMPL(Control, UIElement)

struct Control::Data
{
};

namespace
{
    using itype = QWidget;
}

Control::Control(QWidget *impl)
    : UIElement(impl), d(new Data())
{
}

Control::~Control()
{
    delete d;
}

Rect Control::rect() const
{
    auto w = impl<itype>();
    return Rect(w->x(), w->y(), w->width(), w->height());
}

Control *Control::rect(const Rect &rect)
{
    auto w = impl<itype>();
    w->setGeometry(rect.x, rect.y, rect.w, rect.h);
    return this;
}

Point Control::position() const
{
    auto w = impl<itype>();
    return Convert::toPoint(w->pos());
}

Control *Control::position(const Point &posi)
{
    return rect(Rect(posi, size()));
}

Size Control::size() const
{
    auto w = impl<itype>();
    return Convert::toSize(w->size());
}

Control *Control::size(const Size &s)
{
    return rect(Rect(position(), s));
}

VI_APPFWGUI_NS_END