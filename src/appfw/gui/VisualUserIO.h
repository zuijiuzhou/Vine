#pragma once

#include <appfw/UserIO.h>

#include <appfw/gui/gui_global.h>

VI_APPFWGUI_NS_BEGIN

class VisualUserIO : public UserIO {
    VI_OBJECT_META;
    VI_DISABLE_COPY_MOVE(VisualUserIO);

  public:
    VisualUserIO();
    virtual ~VisualUserIO();

  public:
    virtual void putString(const String& str) override;

    virtual bool getString(String& val, const String& prompt = String::E) const override;
    virtual void getString(String& val, const String& def, const String& prompt = String::E) const override;

    virtual bool getInt(int8_t& val, const String& prompt = String::E) const override;
    virtual void getInt(int8_t& val, int8_t def, const String& prompt = String::E) const override;

    virtual bool getDouble(double& val, const String& prompt = String::E) const override;
    virtual void getDouble(double& val, double def, const String& prompt = String::E) const override;

    virtual bool getPoint3d(ge::Point3d& val, const String& prompt = String::E) const override;
    virtual void getPoint3d(ge::Point3d& val, ge::Point3d& def, const String& prompt = String::E) const override;

  private:
    VI_OBJECT_DATA;
};

VI_APPFWGUI_NS_END