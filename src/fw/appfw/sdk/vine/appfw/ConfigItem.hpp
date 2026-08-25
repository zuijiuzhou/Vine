#pragma once

#include "appfw_global.hpp"
#include <vine/String.hpp>

#include <vector>

V_APPFW_NS_BEGIN

/**
 * \brief 可配置项类型：面板据此选择编辑器。
 */
enum class ConfigItemType {
    String,  ///< 单行文本 → QLineEdit
    Bool,    ///< 开关 → QCheckBox
    Int,     ///< 整数 → QSpinBox
    Double,  ///< 浮点 → QDoubleSpinBox
    Choice,  ///< 枚举选择（存储为 String）→ QComboBox
};

/**
 * \brief 配置项描述符：描述一个可配置项如何展示与编辑。
 *
 * 纯数据、Qt-free。key 为 ConfigManager 中的点分路径；label/description/group
 * 供面板展示；type 决定编辑器；defaultValue/range/step/choices 为编辑约束。
 * 插件经 AddinLoadContext::configs() 或直接向 ConfigRegistry 注册。
 *
 * \note 流式构建（setter 返回自身引用），如：
 * item.group(u8"编辑器").range(8, 72).defaultValue(14)
 */
class V_APPFW_API ConfigItem {
  public:
    ConfigItem(String key, String label, ConfigItemType type);

    ConfigItem(const ConfigItem& other);
    ConfigItem& operator=(const ConfigItem& other);
    ConfigItem(ConfigItem&& other) noexcept;
    ConfigItem& operator=(ConfigItem&& other) noexcept;
    ~ConfigItem();

  public:
    // ---- 只读 ----
    /// 点分 key（ConfigManager 路径）。
    const String& key() const;
    /// 显示名。
    const String& label() const;
    /// 说明（面板 tooltip）。
    const String& description() const;
    /// 分组名（空串 = 未分组）。
    const String& group() const;
    /// 类型。
    ConfigItemType type() const;

    /// 是否配置了默认值。
    bool hasDefault() const;
    /// 默认值（hasDefault 为 true 时有效）。
    const String& defaultString() const;
    bool defaultBool() const;
    int defaultInt() const;
    double defaultDouble() const;

    /// 是否配置了数值范围。
    bool hasRange() const;
    int minInt() const;
    int maxInt() const;
    double minDouble() const;
    double maxDouble() const;
    /// 步进（Int/Double）。
    double step() const;

    /// Choice 的选项列表。
    const std::vector<String>& choices() const;
    /// 是否只读（仅显示不可编辑）。
    bool readOnly() const;

  public:
    // ---- 流式构建 ----
    ConfigItem& description(const String& d);
    ConfigItem& group(const String& g);
    ConfigItem& defaultValue(const String& v);
    ConfigItem& defaultValue(const char8_t* v);
    ConfigItem& defaultValue(bool v);
    ConfigItem& defaultValue(int v);
    ConfigItem& defaultValue(double v);
    ConfigItem& range(int min, int max);
    ConfigItem& range(double min, double max);
    ConfigItem& range(int min, double max);
    ConfigItem& range(double min, int max);
    ConfigItem& step(double s);
    ConfigItem& choices(std::vector<String> cs);
    ConfigItem& readOnly(bool on);

  private:
    struct Data;
    Data* d;
};

V_APPFW_NS_END
