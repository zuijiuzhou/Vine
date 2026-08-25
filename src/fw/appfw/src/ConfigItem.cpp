#include <vine/appfw/ConfigItem.hpp>

#include <cstdint>
#include <utility>

V_APPFW_NS_BEGIN

struct ConfigItem::Data {
    String         key;
    String         label;
    String         description;
    String         group;
    ConfigItemType type = ConfigItemType::String;

    bool    has_default    = false;
    String  default_string;
    bool    default_bool   = false;
    int64_t default_int    = 0;
    double  default_double = 0.0;

    bool                has_range = false;
    double              min_d     = 0.0;
    double              max_d     = 0.0;
    double              step      = 1.0;

    std::vector<String> choices;
    bool                read_only = false;
};

ConfigItem::ConfigItem(String key, String label, ConfigItemType type)
    : d(new Data)
{
    d->key   = std::move(key);
    d->label = std::move(label);
    d->type  = type;
}

ConfigItem::ConfigItem(const ConfigItem& other)
    : d(new Data(*other.d))
{
}

ConfigItem& ConfigItem::operator=(const ConfigItem& other)
{
    if (this != &other) {
        *d = *other.d;
    }
    return *this;
}

ConfigItem::ConfigItem(ConfigItem&& other) noexcept
    : d(other.d)
{
    other.d = nullptr;
}

ConfigItem& ConfigItem::operator=(ConfigItem&& other) noexcept
{
    if (this != &other) {
        delete d;
        d        = other.d;
        other.d  = nullptr;
    }
    return *this;
}

ConfigItem::~ConfigItem()
{
    delete d;
}

// ---- 只读 ----

const String& ConfigItem::key() const { return d->key; }
const String& ConfigItem::label() const { return d->label; }
const String& ConfigItem::description() const { return d->description; }
const String& ConfigItem::group() const { return d->group; }
ConfigItemType ConfigItem::type() const { return d->type; }

bool ConfigItem::hasDefault() const { return d->has_default; }
const String& ConfigItem::defaultString() const { return d->default_string; }
bool ConfigItem::defaultBool() const { return d->default_bool; }
int ConfigItem::defaultInt() const { return static_cast<int>(d->default_int); }
double ConfigItem::defaultDouble() const { return d->default_double; }

bool ConfigItem::hasRange() const { return d->has_range; }
int ConfigItem::minInt() const { return static_cast<int>(d->min_d); }
int ConfigItem::maxInt() const { return static_cast<int>(d->max_d); }
double ConfigItem::minDouble() const { return d->min_d; }
double ConfigItem::maxDouble() const { return d->max_d; }
double ConfigItem::step() const { return d->step; }

const std::vector<String>& ConfigItem::choices() const { return d->choices; }
bool ConfigItem::readOnly() const { return d->read_only; }

// ---- 流式构建 ----

ConfigItem& ConfigItem::description(const String& v)
{
    d->description = v;
    return *this;
}

ConfigItem& ConfigItem::group(const String& v)
{
    d->group = v;
    return *this;
}

ConfigItem& ConfigItem::defaultValue(const String& v)
{
    d->has_default    = true;
    d->default_string = v;
    return *this;
}

ConfigItem& ConfigItem::defaultValue(const char8_t* v)
{
    return defaultValue(String(v));
}

ConfigItem& ConfigItem::defaultValue(bool v)
{
    d->has_default  = true;
    d->default_bool = v;
    return *this;
}

ConfigItem& ConfigItem::defaultValue(int v)
{
    d->has_default   = true;
    d->default_int   = v;
    return *this;
}

ConfigItem& ConfigItem::defaultValue(double v)
{
    d->has_default    = true;
    d->default_double = v;
    return *this;
}

ConfigItem& ConfigItem::range(int min, int max)
{
    return range(static_cast<double>(min), static_cast<double>(max));
}

ConfigItem& ConfigItem::range(double min, double max)
{
    d->has_range = true;
    d->min_d     = min;
    d->max_d     = max;
    return *this;
}

ConfigItem& ConfigItem::range(int min, double max)
{
    return range(static_cast<double>(min), max);
}

ConfigItem& ConfigItem::range(double min, int max)
{
    return range(min, static_cast<double>(max));
}

ConfigItem& ConfigItem::step(double s)
{
    d->step = s;
    return *this;
}

ConfigItem& ConfigItem::choices(std::vector<String> cs)
{
    d->choices = std::move(cs);
    return *this;
}

ConfigItem& ConfigItem::readOnly(bool on)
{
    d->read_only = on;
    return *this;
}

V_APPFW_NS_END
