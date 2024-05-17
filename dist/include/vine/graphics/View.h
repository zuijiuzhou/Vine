#include "graphics_global.h"

#include "core/Object.h"

#include "ge/Matrix4x4.h"
#include "ge/Point3d.h"
#include "ge/Vector3d.h"


VI_GRAPHICS_NS_BEGIN

class VI_GRAPHICS_API View : public Object
{
    VI_OBJECT_META
public:
    void addModel() const;
};
using ViewPtr = RefPtr<View>;

VI_GRAPHICS_NS_END