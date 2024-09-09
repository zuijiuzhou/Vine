#include <gtest/gtest.h>
#include <vine/core/String.h>

namespace {
class Base1 {
  public:
    void function() { std::cout << "Base1 function" << std::endl; }
};

class Base2 {
  public:
    void function() { std::cout << "Base2 function" << std::endl; }
};

class Derived : public Base1, public Base2 {
  public:
    void function() { std::cout << "Derived function" << std::endl; }
};
} // namespace



TEST(TestCppStd, PointerCast) {
    Derived d;
    Base1*  b1 = (Base1*)&d;
    Base2*  b2 = (Base2*)&d;
    std::cout << b1 << std::endl;
    std::cout << b2 << std::endl;
    ASSERT_EQ((void*)b1, b2);
}