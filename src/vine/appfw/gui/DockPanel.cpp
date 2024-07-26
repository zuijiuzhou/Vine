#include <QDockWidget>
#include <QHBoxLayout>

#include <vine/appfw/gui/DockPanel.h>

#include "Convert.h"

VI_APPFWGUI_NS_BEGIN
VI_OBJECT_META_IMPL(DockPanel, Control)

namespace {
using itype = QDockWidget;

struct DockPanelTitleBar : public QWidget {
    DockPanelTitleBar() {

    }
    
    QHBoxLayout* layout = nullptr;
};
} // namespace

struct DockPanel::Data {
    RefPtr<Control> content;

    QDockWidget*       impl     = nullptr;
    DockPanelTitleBar* titlebar = nullptr;
};

DockPanel::DockPanel()
  : Control(new QDockWidget())
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

void DockPanel::setContent(Control* content) {
    d->impl->setWidget(content->impl<QWidget>());
    d->content = content;
}

Control* DockPanel::getContent() const {
    return d->content.get();
}

VI_APPFWGUI_NS_END