#include <vine/appfw/gui/GuiApplication.hpp>

#include <QApplication>
#include <QStyleHints>

#include <vine/appfw/gui/GuiApplicationData.hpp>

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(GuiApplication, Application)

GuiApplication::GuiApplication(int argc, char** argv)
  : Application(new GuiApplicationData(), argc, argv)
{}

GuiApplication::~GuiApplication()
{
    // d is deleted by Application::~Application()
}

void GuiApplication::init()
{
    auto* d = static_cast<GuiApplicationData*>(dptr());
    if (d->app == nullptr) {
        int c  = this->argc();
        d->app = new QApplication(c, argv());
        QApplication::styleHints()->setColorScheme(Qt::ColorScheme::Light);
    }
}

int GuiApplication::run()
{
    auto* d = static_cast<GuiApplicationData*>(dptr());
    return d->app->exec();
}

V_APPFWGUI_NS_END
