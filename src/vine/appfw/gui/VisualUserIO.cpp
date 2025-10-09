#include "VisualUserIO.hpp"

#include <QDockWidget>

VI_APPFWGUI_NS_BEGIN

VI_OBJECT_META_IMPL(VisualUserIO, UserIO)

struct VisualUserIO::Data {};

VisualUserIO::VisualUserIO()
  : d(new Data()) {
}

VisualUserIO::~VisualUserIO() {
}

void VisualUserIO::putString(const String& str) {
}

bool VisualUserIO::getString(String& val, const String& prompt) const {
    return false;
}

void VisualUserIO::getString(String& val, const String& def, const String& prompt) const {
}

bool VisualUserIO::getInt(int8_t& val, const String& prompt) const {
    return false;
}

void VisualUserIO::getInt(int8_t& val, int8_t def, const String& prompt) const {
}

bool VisualUserIO::getDouble(double& val, const String& prompt) const {
    return false;
}

void VisualUserIO::getDouble(double& val, double def, const String& prompt) const {
}

bool VisualUserIO::getPoint3d(ge::Point3d& val, const String& prompt) const {
    return false;
}

void VisualUserIO::getPoint3d(ge::Point3d& val, ge::Point3d& def, const String& prompt) const {
}

VI_APPFWGUI_NS_END
