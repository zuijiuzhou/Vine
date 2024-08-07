#pragma once
#include "appfw_global.h"

#include <vine/core/Object.h>

#include <vine/core/String.h>
#include <vine/core/core_defs.h>
#include <vine/ge/ge_global.h>

VI_GE_NS_BEGIN
class Point3d;
VI_GE_NS_END

VI_APPFW_NS_BEGIN

class VI_APPFW_API UserIO : public Object {
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

    virtual bool getPoint3d(ge::Point3d& val, const String& prompt = String::E) const                   = 0;
    virtual void getPoint3d(ge::Point3d& val, ge::Point3d& def, const String& prompt = String::E) const = 0;
};
using UserIOPtr = RefPtr<UserIO>;

VI_APPFW_NS_END