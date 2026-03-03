#include <gtest/gtest.h>
#include <vine/String.hpp>

TEST(String, fromUtf8)
{
    vine::String str;
    str = U"你好123";
    str.assign(U"123你好");

    auto str2 = str.substr(3,2);
    auto p = str.find(U'3');

    size_t xx{};

}