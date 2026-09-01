#include "graphics_global.hpp"

#include <vine/intrusive_ptr.hpp>
#include <vine/Object.hpp>
#include <vine/RefCounted.hpp>

V_GRAPHICS_NS_BEGIN

class Scene;

class V_GRAPHICS_API View : public Object, public RefCounted<View> {
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

using ViewPtr = intrusive_ptr<View>;

V_GRAPHICS_NS_END
