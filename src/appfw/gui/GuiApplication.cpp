#include <QApplication>

#include <appfw/gui/GuiApplication.h>

ETD_APPFW_GUI_BEGIN

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

ETD_APPFW_GUI_NS_END