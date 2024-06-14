#include <iostream>
#include <array>
#include <ctime>
#include <vector>

#include <ge/Point3d.h>
#include <ge/Vector3d.h>

#include <core/core_stl.h>

#include <di/Registration.h>
#include <di/Container.h>

#include <appfw/AddinManager.h>
#include <appfw/gui/GuiApplication.h>
#include <appfw/gui/MainWindow.h>

#include <appfw/gui/RibbonBar.h>
#include <appfw/gui/RibbonTab.h>
#include <appfw/gui/RibbonGroup.h>
#include <appfw/gui/RibbonDropDownItem.h>

namespace fw = vine::appfw;
namespace guifw = fw::gui;

class Base1 {
public:
    virtual ~Base1() {
        std::cout << "Base1 destructor called" << std::endl;
    }
};

class Base2 {
public:
    virtual ~Base2() {
        std::cout << "Base2 destructor called" << std::endl;
    }
};

class Derived : public Base1, public Base2 {
public:
    ~Derived() override {
        std::cout << "Derived destructor called" << std::endl;
    }
};



int main(int argc, char **argv)
{
    Base1* b1 = new Derived();
    delete b1;  // 输出 Derived destructor called 和 Base1 destructor called

    Base2* b2 = new Derived();
    delete b2;  // 输出 Derived destructor called 和 Base2 destructor called
    
    vine::ge::Vector3d v(1,2,3);

    vine::ge::Point3d p = v.asPoint();

    guifw::GuiApplicationPtr app = new guifw::GuiApplication(argc, argv);
    app->init();
    guifw::MainWindowPtr wnd = new guifw::MainWindow();
    wnd->show();

    vine::di::Container c;
    auto reg = vine::di::Registration::create<vine::Object>()->impl<fw::AddinManager>()->lifetime(vine::di::Lifetime::Singleton);
    c.add(reg);

    auto bar = wnd->ribbonBar();
    auto rtab = new guifw::RibbonTab();
    rtab->title(U"Tab1");
    bar->addTab(rtab);
    auto rgroup = new guifw::RibbonGroup();
    rgroup->title(U"Group1");
    rtab->addGroup(rgroup);

    auto mi1 = new guifw::RibbonDropDownItem();
    mi1->text(U"Open")->data((void*)123);
    bar->appendApplicationMenu(mi1);

    return app->run();
}