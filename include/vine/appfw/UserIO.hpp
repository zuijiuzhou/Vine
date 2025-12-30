#pragma once
#include "appfw_global.hpp"

#include <cstdint>

#include <vine/core/RefObject.hpp>
#include <vine/core/String.hpp>
#include <vine/math/math_global.hpp>

VI_MATH_NS_BEGIN
class Point3d;
VI_MATH_NS_END

VI_APPFW_NS_BEGIN

class VI_APPFW_API UserIO : public RefObject {
    VI_OBJECT_META;
    VI_DISABLE_COPY_MOVE(UserIO);

  public:
    UserIO();

  public:
    virtual void putString(const String& str) = 0;

    virtual bool getString(String& val, const String& prompt = String::E) const                    = 0;
    virtual void getString(String& val, const String& def, const String& prompt = String::E) const = 0;

    virtual bool getInt(int8_t& val, const String& prompt = String::E) const             = 0;
    virtual void getInt(int8_t& val, int8_t def, const String& prompt = String::E) const = 0;

    virtual bool getDouble(double& val, const String& prompt = String::E) const             = 0;
    virtual void getDouble(double& val, double def, const String& prompt = String::E) const = 0;

    virtual bool getPoint3d(math::Point3d& val, const String& prompt = String::E) const                   = 0;
    virtual void getPoint3d(math::Point3d& val, math::Point3d& def, const String& prompt = String::E) const = 0;
};
using UserIOPtr = RefPtr<UserIO>;

VI_APPFW_NS_END