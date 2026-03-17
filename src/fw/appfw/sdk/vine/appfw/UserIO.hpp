#pragma once
#include "appfw_global.hpp"

#include <cstdint>

#include <vine/RefObject.hpp>
#include <vine/String.hpp>
#include <vine/math/math_global.hpp>

V_MATH_NS_BEGIN
class Point3d;
V_MATH_NS_END

V_APPFW_NS_BEGIN

class V_APPFW_API UserIO : public RefObject {
    V_OBJECT_META_DECL;

  public:
    UserIO();

  public:
    virtual void putString(const String& str) = 0;

    virtual bool getString(String& val, const String& prompt = {}) const                    = 0;
    virtual void getString(String& val, const String& def, const String& prompt = {}) const = 0;

    virtual bool getInt(int8_t& val, const String& prompt = {}) const             = 0;
    virtual void getInt(int8_t& val, int8_t def, const String& prompt = {}) const = 0;

    virtual bool getDouble(double& val, const String& prompt = {}) const             = 0;
    virtual void getDouble(double& val, double def, const String& prompt = {}) const = 0;

    virtual bool getPoint3d(math::Point3d& val, const String& prompt = {}) const                     = 0;
    virtual void getPoint3d(math::Point3d& val, math::Point3d& def, const String& prompt = {}) const = 0;
};

using UserIOPtr = RefPtr<UserIO>;

V_APPFW_NS_END
