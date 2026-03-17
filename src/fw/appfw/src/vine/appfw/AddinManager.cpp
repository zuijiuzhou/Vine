#include <vine/appfw/AddinManager.hpp>

#include <vector>

#ifdef __MSVC__
#    include <Windows.h>
#endif // __MSVC__


V_APPFW_NS_BEGIN

V_OBJECT_META_IMPL(AddinManager, RefObject)

struct AddinManager::Data {};

AddinManager::AddinManager()
  : d(new Data())
{}

Addin* AddinManager::load(const String& str)
{
    return nullptr;
}

V_APPFW_NS_END
