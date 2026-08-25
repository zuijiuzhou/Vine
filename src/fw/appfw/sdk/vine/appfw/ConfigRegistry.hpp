#pragma once

#include "ConfigItem.hpp"
#include "appfw_global.hpp"
#include <vine/String.hpp>

#include <map>
#include <vector>

V_APPFW_NS_BEGIN

/**
 * \brief 配置项注册表：插件在此注册可显示的配置项（ConfigItem）。
 *
 * 只存元数据，不存值（值在 ConfigManager）；按注册顺序保持显示顺序，
 * 并提供 key 索引查询与分组列表。Qt-free。
 */
class V_APPFW_API ConfigRegistry {
  public:
    ConfigRegistry();
    ~ConfigRegistry();

    /// 注册配置项；同 key 已存在则拒绝，返回 false。
    bool addItem(const ConfigItem& item);
    /// 移除指定 key 的配置项；不存在返回 false。
    bool removeItem(const String& key);
    /// 清空。
    void clear();

    /// 已注册数量。
    int itemCount() const;
    /// 按注册顺序取第 index 项；越界返回 nullptr。
    const ConfigItem* itemAt(int index) const;
    /// 按 key 查询；不存在返回 nullptr。
    const ConfigItem* item(const String& key) const;
    /// 全部条目（注册顺序）。
    const std::vector<ConfigItem>& items() const;
    /// 去重后的分组列表（按首次出现顺序；未分组为空串）。
    std::vector<String> groups() const;

  private:
    struct Data;
    Data* const d;
};

V_APPFW_NS_END
