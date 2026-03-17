#include <vine/appfw/Addin.hpp>

V_APPFW_NS_BEGIN

V_OBJECT_META_IMPL(Addin, RefObject)

void Addin::load(AddinLoadContext* context)
{}

void Addin::unload()
{}

String Addin::getName() const
{
    return String();
}

V_APPFW_NS_END
