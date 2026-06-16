#pragma once

#include <vine/String.hpp>

#include <QObject>

V_APPFWGUI_NS_BEGIN

struct UIElementData {
    String                  name;
    QObject*                impl         = nullptr;
    bool                    impl_deleted = false;
    bool                    owns_impl    = true;
    QMetaObject::Connection impl_destroyed_connection;

    virtual ~UIElementData() = default;
};

V_APPFWGUI_NS_END
