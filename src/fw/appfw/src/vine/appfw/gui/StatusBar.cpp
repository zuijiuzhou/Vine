#include <vine/appfw/gui/StatusBar.hpp>

#include <QStatusBar>

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(StatusBar, UIElement)

struct StatusBar::Data {
    QStatusBar* sb = nullptr;
};

StatusBar::StatusBar()
  : UIElement(new QStatusBar())
  , d(new Data)
{
    d->sb = impl<QStatusBar>();
}

StatusBar::StatusBar(UIElement* parent)
  : UIElement(new QStatusBar())
  , d(new Data)
{
    d->sb = impl<QStatusBar>();
}

StatusBar::~StatusBar()
{
    delete d;
}

void StatusBar::showMessage(const String& msg, int timeout_ms)
{
    auto utf16 = msg.toUtf16();
    d->sb->showMessage(QString::fromUtf16(reinterpret_cast<const ushort*>(utf16.data()), (int)utf16.size()), timeout_ms);
}

V_APPFWGUI_NS_END
