#include "graphics_global.h"

#include <vine/core/Object.h>

VI_GRAPHICS_NS_BEGIN

class Scene;
class VI_GRAPHICS_API View : public Object {
  VI_OBJECT_META;
  VI_DISABLE_MOVE(View);

public:
  View();

public:
  void setScene(Scene* scene);

private:
  VI_OBJECT_DATA;
};
using ViewPtr = RefPtr<View>;

VI_GRAPHICS_NS_END