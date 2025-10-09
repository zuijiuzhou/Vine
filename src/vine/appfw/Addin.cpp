#include <vine/appfw/Addin.hpp>

VI_APPFW_NS_BEGIN

VI_OBJECT_META_IMPL(Addin, Object)

void Addin::Load(AddinLoadContext* context)
{
}

void Addin::Unload() {

}

String Addin::getName() const {
    return String();
}

VI_APPFW_NS_END

