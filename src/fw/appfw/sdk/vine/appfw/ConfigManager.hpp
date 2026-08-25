#pragma once

#include <vine/appfw/appfw_global.hpp>
#include <vine/Events.hpp>
#include <vine/String.hpp>

#include <vector>

V_APPFW_NS_BEGIN

/**
 * \brief 配置变更事件参数：携带发生变更的 key（点分路径）。
 */
class V_APPFW_API ConfigChangedEventArgs : public EventArgs {
    V_OBJECT_META_DECL

  public:
    explicit ConfigChangedEventArgs(const String& key);
    /// 发生变更的 key。
    const String& key() const;

  private:
    String key_;
};

/**
 * \brief 配置管理器：按 key 存取 String/bool/int/double 标量与数组，支持 JSON 序列化。
 *
 * Application 持有唯一实例（Application::configManager()）。
 * \note 标量 setter/getter 用 set/get 前缀：setter 与 getter 参数同型，
 * 无前缀重载会歧义，故与 gui 层无前缀风格不同。
 * \note key 支持点分层级，如 u8"window.x" 表示 window 下的 x；
 * toJson/loadJson 按层级生成/读取嵌套 JSON 对象。
 */
class V_APPFW_API ConfigManager {
  public:
    ConfigManager();
    virtual ~ConfigManager();

  public:
    /// 配置变更事件：set*/remove/clear 触发，参数携带 key。
    Event<ConfigManager, ConfigChangedEventArgs> changed;

  public:
    // ---- 存在性 / 生命周期 ----
    /// 是否包含指定 key。
    bool contains(const String& key) const;
    /// 移除指定 key。
    void remove(const String& key);
    /// 清空所有配置。
    void clear();

  public:
    // ---- 标量 ----
    void setString(const String& key, const String& value);
    String getString(const String& key, const String& def = String()) const;

    void setBool(const String& key, bool value);
    bool getBool(const String& key, bool def = false) const;

    void setInt(const String& key, int value);
    int getInt(const String& key, int def = 0) const;

    void setDouble(const String& key, double value);
    double getDouble(const String& key, double def = 0.0) const;

  public:
    // ---- 数组 ----
    void setStringArray(const String& key, const std::vector<String>& values);
    std::vector<String> getStringArray(const String& key) const;

    void setBoolArray(const String& key, const std::vector<bool>& values);
    std::vector<bool> getBoolArray(const String& key) const;

    void setIntArray(const String& key, const std::vector<int>& values);
    std::vector<int> getIntArray(const String& key) const;

    void setDoubleArray(const String& key, const std::vector<double>& values);
    std::vector<double> getDoubleArray(const String& key) const;

  public:
    // ---- 序列化 ----
    /// 导出为 JSON 字符串（类型标记格式，可无损往返）。
    String toJson() const;
    /// 从 JSON 字符串加载（替换现有配置）。返回是否成功。
    bool loadJson(const String& json);
    /// 保存到文件（UTF-8 JSON）。
    bool save(const String& path) const;
    /// 从文件加载。
    bool load(const String& path);

  private:
    struct Data;
    Data* const d;
};

V_APPFW_NS_END
