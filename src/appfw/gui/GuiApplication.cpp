#include <QApplication>

#include <appfw/gui/GuiApplication.h>

ETD_APPFW_GUI_BEGIN

struct GuiApplication::Data
{
    QApplication *app = nullptr;
};

GuiApplication::GuiApplication(int argc, char **argv)
    : Inherit(argc, argv), d_ptr(new Data)
{
}
GuiApplication::~GuiApplication()
{
    delete d_ptr;
}

void GuiApplication::init()
{
    if (d_ptr->app == nullptr)
    {
        int c = this->argc();
        d_ptr->app = new QApplication(c, argv());
    }
}

int GuiApplication::run()
{
    return d_ptr->app->exec();
}

ETD_APPFW_GUI_NS_END