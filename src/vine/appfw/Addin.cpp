#include <vine/appfw/Addin.hpp>

VI_APPFW_NS_BEGIN

VI_OBJECT_META_IMPL(Addin, RefObject)

void Addin::load(AddinLoadContext* context)
{
}

void Addin::unload() {

}

String Addin::getName() const {
    return String();
}

VI_APPFW_NS_END

