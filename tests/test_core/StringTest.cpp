#include <gtest/gtest.h>
#include <vine/String.hpp>

TEST(String, fromUtf8)
{
    vine::String str;
    str = u8"你好123";
    str.assign(u8"123你好");

    auto str2 = str.substr(3,2);
    auto p = str.find(u'3');

    // auto str3 = vine::String::fromUtf8(u8"你好123大大家");
    // auto str4 = vine::String::fromUtf16(u"你好123大大家");
    // // auto str5 = vine::String::fromLocal8Bit("你好123大大家");

    // auto str6 = str4.toUtf8();
    // auto str7 = str4.toUtf16();

    size_t xx{};

}