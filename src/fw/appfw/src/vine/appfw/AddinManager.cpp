#include <vine/appfw/AddinManager.hpp>

#include <vector>

#ifdef __MSVC__
#include <Windows.h>
#endif // __MSVC__


VI_APPFW_NS_BEGIN

VI_OBJECT_META_IMPL(AddinManager, RefObject)

struct AddinManager::Data {
	
};

AddinManager::AddinManager()
  : d(new Data()) {

}

Addin* AddinManager::load(const String& str) {
    return nullptr;
}

VI_APPFW_NS_END
