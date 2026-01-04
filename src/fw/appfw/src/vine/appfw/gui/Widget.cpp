#include <vine/appfw/gui/Widget.hpp>

#include <QWidget>

#include "vine/appfw/gui/Convert.hpp"

VI_APPFWGUI_NS_BEGIN

VI_OBJECT_META_IMPL(Widget, UIElement)

struct Widget::Data {};

namespace {
using itype = QWidget;
}

Widget::Widget(QWidget* impl)
  : UIElement(impl)
  , d(new Data()) {
}

Widget::~Widget() {
    delete d;
}

Rect Widget::rect() const {
    auto w = impl<itype>();
    return Rect(w->x(), w->y(), w->width(), w->height());
}

void Widget::rect(const Rect& rect) {
    auto w = impl<itype>();
    w->setGeometry(rect.x, rect.y, rect.w, rect.h);
}

Point Widget::position() const {
    auto w = impl<itype>();
    return Convert::toPoint(w->pos());
}

void Widget::position(const Point& posi) {
    rect(Rect(posi, size()));
}

Size Widget::size() const {
    auto w = impl<itype>();
    return Convert::toSize(w->size());
}

void Widget::size(const Size& s) {
    rect(Rect(position(), s));
}

VI_APPFWGUI_NS_END