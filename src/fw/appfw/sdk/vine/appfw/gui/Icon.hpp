#pragma once

#include <vine/appfw/appfw_global.hpp>
#include <vine/String.hpp>

class QIcon;

V_APPFWGUI_NS_BEGIN

/**
 * \brief 图标对象：内部持有 QIcon（Pimpl 隐藏，头文件不包含 Qt 头）。
 *
 * 供 RibbonButton / RibbonAction 等 gui 包装类设置图标，让框架层
 * 头文件不暴露、不包含 Qt。可从现有 QIcon 或文件路径（String）构造。
 *
 * \note 这是框架内唯一接触 QIcon 的桥接类型：QIcon 在头文件中仅前置声明，
 * 需要真正使用 QIcon 的代码（实现文件）自行包含 <QIcon>。
 */
class V_APPFW_API Icon {
  public:
    Icon();
    explicit Icon(const QIcon& qicon);
    explicit Icon(const String& path);
    Icon(const Icon& other);
    Icon& operator=(const Icon& other);
    ~Icon();

  public:
    /// 底层 QIcon（只读）。
    const QIcon& value() const;

    /// 是否为空图标。
    bool isNull() const;

    /// 隐式转换为 QIcon。
    operator QIcon() const;

  private:
    struct Data;
    Data* d;
};

V_APPFWGUI_NS_END
