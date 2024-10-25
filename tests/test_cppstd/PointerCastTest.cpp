#include <gtest/gtest.h>
#include <vine/core/String.h>

namespace {
class Base1 {
  public:
  virtual~Base1(){}
    std::string  name ="Base1";
    void function() { std::cout << "Base1 function" << std::endl; }
};

class Base2 {
  public:
  virtual~Base2(){}
    std::string  name ="Base2";
    void function() { std::cout << "Base2 function" << std::endl; }
};

class Derived : public Base1, public Base2 {
  public:
    std::string  name ="Derived";
    void function() { std::cout << "Derived function" << std::endl; }
};

class DerivedFinal : public Derived{
  public:
  std::string name = "DrivedFinal";
};
} // namespace



TEST(TestCppStd, PointerCast) {
    Derived d;
    auto df = new DerivedFinal();
    auto df_b1 = static_cast<Base1*>(df);
    auto df_b2 = static_cast<Base2*>(df);
    auto df_d = static_cast<Derived*>(df);
    auto df2 = static_cast<DerivedFinal*>(df_b2);



    Base1*  b1 = (Base1*)&d;
    Base2*  b2 = (Base2*)&d;
    std::cout << b1 << std::endl;
    std::cout << b2 << std::endl;
    ASSERT_EQ((void*)b1, b2);
}