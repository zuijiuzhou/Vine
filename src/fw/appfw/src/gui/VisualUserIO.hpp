#pragma once

#include <vine/appfw/UserIO.hpp>

V_APPFWGUI_NS_BEGIN

class VisualUserIO : public UserIO {
    V_OBJECT_META_DECL;
    V_DISABLE_COPY_MOVE(VisualUserIO);

  public:
    VisualUserIO();
    virtual ~VisualUserIO();

  public:
    virtual void putString(const String& str) override;

    virtual bool getString(String& val, const String& prompt = {}) const override;
    virtual void getString(String& val, const String& def, const String& prompt = {}) const override;

    virtual bool getInt(int8_t& val, const String& prompt = {}) const override;
    virtual void getInt(int8_t& val, int8_t def, const String& prompt = {}) const override;

    virtual bool getDouble(double& val, const String& prompt = {}) const override;
    virtual void getDouble(double& val, double def, const String& prompt = {}) const override;

    virtual bool getPoint3d(math::Point3d& val, const String& prompt = {}) const override;
    virtual void getPoint3d(math::Point3d& val, math::Point3d& def, const String& prompt = {}) const override;

  private:
    struct Data;
    Data* const d;
    ;
};

V_APPFWGUI_NS_END
