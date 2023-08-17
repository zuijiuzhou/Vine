#include <QApplication>

#include <appfw/gui/GuiApplication.h>

VINE_APPFWGUI_NS_BEGIN

struct GuiApplication::Data
{
    QApplication *app = nullptr;
};

GuiApplication::GuiApplication(int argc, char **argv)
    : Inherit(argc, argv), d(new Data)
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

VINE_APPFWGUI_NS_END