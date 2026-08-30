#include <gtest/gtest.h>

#include <vine/ICloneable.hpp>
#include <vine/IComparable.hpp>
#include <vine/IStringable.hpp>

using vine::IComparable;
using vine::ICloneable;
using vine::IStringable;

namespace
{

class Widget : public vine::Object, public IStringable, public IComparable, public ICloneable {
  public:
    V_OBJECT_META(Widget, vine::Object, IStringable, IComparable, ICloneable)

    int value{ 0 };

    vine::String toString() const override
    {
        return value == 0 ? vine::String(u8"zero") : vine::String(u8"other");
    }

    int compareTo(const vine::Object& other) const override
    {
        const auto* w = vine::obj_cast<const Widget>(&other);
        return w ? value - w->value : 0;
    }

    vine::Object* clone() const override
    {
        return new Widget(*this);
    }
};

} // namespace

TEST(InterfaceTest, KindOfCommonInterface)
{
    Widget w;
    vine::Object* obj = &w;
    EXPECT_TRUE(obj->isKindOf<IStringable>());
    EXPECT_TRUE(obj->isKindOf<IComparable>());
    EXPECT_TRUE(obj->isKindOf<ICloneable>());
    EXPECT_TRUE(obj->isKindOf<Widget>());
    EXPECT_TRUE(obj->isKindOf(vine::Object::desc()));
}

TEST(InterfaceTest, ObjCastToCommonInterface)
{
    Widget w;
    w.value = 7;
    vine::Object* obj = &w;

    auto* s = obj_cast<IStringable>(obj);
    ASSERT_NE(s, nullptr);
    EXPECT_EQ(s->toString().size(), 5u);

    auto* c = obj_cast<IComparable>(obj);
    ASSERT_NE(c, nullptr);

    auto* cl = obj_cast<ICloneable>(obj);
    ASSERT_NE(cl, nullptr);
    vine::Object* copy = cl->clone();
    ASSERT_NE(copy, nullptr);
    delete copy;
}

TEST(InterfaceTest, ConceptChecks)
{
    EXPECT_TRUE(vine::Stringable<Widget>);
    EXPECT_TRUE(vine::Comparable<Widget>);
    EXPECT_TRUE(vine::Cloneable<Widget>);
}
