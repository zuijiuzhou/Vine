#include <vine/appfw/gui/GuiApplication.hpp>

#include <QApplication>

VI_APPFWGUI_NS_BEGIN

VI_OBJECT_META_IMPL(GuiApplication, Application)

struct GuiApplication::Data
{
    QApplication *app = nullptr;
};

GuiApplication::GuiApplication(int argc, char **argv)
    : Application(argc, argv), d(new Data)
{
}
GuiApplication::~GuiApplication()
{
    delete d;
}

void GuiApplication::init()
{
    if (d->app == nullptr)
    {
        int c = this->argc();
        d->app = new QApplication(c, argv());
    }
}

int GuiApplication::run()
{
    return d->app->exec();
}

VI_APPFWGUI_NS_END