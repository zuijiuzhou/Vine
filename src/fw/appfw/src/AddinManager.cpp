#include <vine/appfw/AddinManager.hpp>

#ifdef V_CC_MSVC
#    include <Windows.h>
#endif // V_CC_MSVC


V_APPFW_NS_BEGIN


struct AddinManager::Data {};

AddinManager::AddinManager()
  : d(new Data())
{}

Addin* AddinManager::load(const String& str)
{
    return nullptr;
}

V_APPFW_NS_END
