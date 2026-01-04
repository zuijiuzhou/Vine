#include <vine/appfw/gui/DockPanel.hpp>

#include <QDockWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QEvent>

#include "vine/appfw/gui/Convert.hpp"

VI_APPFWGUI_NS_BEGIN
VI_OBJECT_META_IMPL(DockPanel, Widget)

namespace {
using itype = QDockWidget;

struct DockPanelTitleBar : public QWidget {
    DockPanelTitleBar() {}

    QHBoxLayout* layout = nullptr;
};

struct QDockWidgetEx : public QDockWidget {
    QDockWidgetEx() {
        auto c = new QWidget();
        c->setStyleSheet("border:1px solid red");
        c->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        setWidget(c);
        setAllowedAreas(Qt::DockWidgetArea::AllDockWidgetAreas);

        auto t = new QWidget();
        t->setStyleSheet("border:1px solid green");
        
        auto t_l = new QHBoxLayout();
        auto t_lbl = new QLabel("123");
        t_l->addWidget(t_lbl);
        t->setLayout(t_l);
        setTitleBarWidget(t);
        this->c = c;
    }

    virtual bool event(QEvent* event) override {
       /* if (event->type() == QEvent::Resize) {
            c->setMaximumWidth(width());
            c->setMaximumHeight(height());
        }*/
        return QDockWidget::event(event);
    }

    QWidget* c;
};

} // namespace

struct DockPanel::Data {
    RefPtr<Widget> content;

    QDockWidget*       impl     = nullptr;
    DockPanelTitleBar* titlebar = nullptr;
};

DockPanel::DockPanel()
  : Widget(new QDockWidgetEx())
  , d(new Data()) {
    d->impl = impl<QDockWidget>();
}

DockPanel::~DockPanel() {
    delete d;
}

void DockPanel::setAllowedAreas(DockAreas areas) {
    auto qareas = Convert::toQDockAreas(areas);
    d->impl->setAllowedAreas(qareas);
}
DockAreas DockPanel::getAllowedAreas() const {
    auto areas = Convert::toDockAreas(impl<itype>()->allowedAreas());
    return areas;
}

void DockPanel::setFeatures(DockFeatures features) {
    auto qfeatures = Convert::toQDockFeatures(features);
    d->impl->setFeatures(qfeatures);
}
DockFeatures DockPanel::getFeatures() const {
    auto features = Convert::toDockFeatures(impl<itype>()->features());
    return features;
}

void DockPanel::setTitle(const String& title) {
    impl<itype>()->setWindowTitle(QString::fromUcs4(title.data()));
}
String DockPanel::getTitle() const {
    return String((char32_t*)impl<itype>()->windowTitle().toUcs4().data());
}

void DockPanel::setContent(Widget* content) {
    d->impl->setWidget(content->impl<QWidget>());
    d->content = content;
}

Widget* DockPanel::getContent() const {
    return d->content.get();
}

VI_APPFWGUI_NS_END