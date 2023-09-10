#include <appfw/gui/RibbonGallery.h>
#include <SARibbon.h>

VI_APPFWGUI_NS_BEGIN

VI_OBJECT_META_IMPL(RibbonGallery, UIElement);

struct RibbonGallery::Data
{
};

namespace
{
    using itype = SARibbonGallery;
}

RibbonGallery::RibbonGallery()
    : UIElement(new QAction()), d(new Data())
{
}

RibbonGallery::~RibbonGallery()
{
    delete d;
}

VI_APPFWGUI_NS_END
