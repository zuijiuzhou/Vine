#pragma once

#include "UIElement.h"

VI_APPFWGUI_NS_BEGIN

class VI_APPFWGUI_API Control : public UIElement
{
    VI_OBJECT_META

protected:
    Control(void* impl);

private:
    VI_DISABLE_COPY_MOVE(Control);

public:

    Rect rect() const;
    Control* rect(const Rect& rect);


private:
    VI_OBJECT_DATA
};

VI_APPFWGUI_NS_END