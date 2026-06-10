#pragma once

#include <vine/String.hpp>

#include <QObject>

V_APPFWGUI_NS_BEGIN

struct UIElementData {
    String                  name;
    QObject*                impl         = nullptr;
    bool                    impl_deleted = false;
    QMetaObject::Connection impl_destroyed_connection;
};

V_APPFWGUI_NS_END
