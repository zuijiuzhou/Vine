#include <appfw/gui/Control.h>

#include <QWidget>

VINE_APPFWGUI_NS_BEGIN

VI_OBJECT_META_IMPL(Control, UIElement)

struct Control::Data
{

};

namespace{
    using itype = QWidget;
}

Control::Control(void* impl)
    : UIElement(impl)
    , d(new Data())
{

}

Rect Control::rect() const{
    auto w = impl<itype>();
    return Rect(w->x(), w->y(), w->width(), w->height());
}

Control* Control::rect(const Rect& rect){
    auto w = impl<itype>();
    w->setGeometry(rect.x, rect.y, rect.w, rect.h);
    return this;
}

VINE_APPFWGUI_NS_END