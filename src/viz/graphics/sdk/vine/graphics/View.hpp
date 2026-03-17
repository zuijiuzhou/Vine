#include "graphics_global.hpp"

#include <vine/RefObject.hpp>

V_GRAPHICS_NS_BEGIN

class Scene;

class V_GRAPHICS_API View : public RefObject {
    V_OBJECT_META_DECL;
    V_DISABLE_MOVE(View);

  public:
    View();

  public:
    void setScene(Scene* scene);

  private:
    struct Data;
    Data* const d;
    ;
};

using ViewPtr = RefPtr<View>;

V_GRAPHICS_NS_END
