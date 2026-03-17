#include <vine/appfw/gui/RibbonGallery.hpp>

#include <SARibbon.h>

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(RibbonGallery, UIElement);

struct RibbonGallery::Data {};

namespace
{

using itype = SARibbonGallery;

}

RibbonGallery::RibbonGallery()
  : UIElement(new QAction())
  , d(new Data())
{}

RibbonGallery::~RibbonGallery()
{
    delete d;
}

V_APPFWGUI_NS_END
