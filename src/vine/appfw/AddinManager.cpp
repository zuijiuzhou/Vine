#include <vine/appfw/AddinManager.h>

#include <vector>

#ifdef __MSVC__
#include <Windows.h>
#endif // __MSVC__


VI_APPFW_NS_BEGIN

VI_OBJECT_META_IMPL(AddinManager, Object)

struct AddinManager::Data {
	
};

AddinManager::AddinManager()
  : d(new Data()) {

}

Addin* AddinManager::load(const String& str) {
    return nullptr;
}

VI_APPFW_NS_END
