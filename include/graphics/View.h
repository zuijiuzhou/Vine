#include "graphics_global.h"

#include "core/Inherit.h"
#include "ge/Matrix4x4.h"
#include "ge/Point3d.h"
#include "ge/Vector3d.h"


VINE_GRAPHICS_NS_BEGIN

class VINE_GRAPHICS_API View : public Inherit<Object, View>
{
public:
    void addModel() const;
};

VINE_GRAPHICS_NS_END