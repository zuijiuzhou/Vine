#include <vine/appfw/ConfigRegistry.hpp>

#include <cstddef>

V_APPFW_NS_BEGIN

struct ConfigRegistry::Data {
    std::vector<ConfigItem> items;             // 有序（注册顺序 = 显示顺序）
    std::map<String, size_t> index;            // key -> index
};

ConfigRegistry::ConfigRegistry()
    : d(new Data)
{
}

ConfigRegistry::~ConfigRegistry()
{
    delete d;
}

bool ConfigRegistry::addItem(const ConfigItem& item)
{
    if (d->index.find(item.key()) != d->index.end())
        return false;   // 重复 key，拒绝
    d->index.emplace(item.key(), d->items.size());
    d->items.push_back(item);
    return true;
}

bool ConfigRegistry::removeItem(const String& key)
{
    auto it = d->index.find(key);
    if (it == d->index.end())
        return false;
    d->items.erase(d->items.begin() + static_cast<std::ptrdiff_t>(it->second));
    // 重建索引
    d->index.clear();
    for (size_t i = 0; i < d->items.size(); ++i)
        d->index.emplace(d->items[i].key(), i);
    return true;
}

void ConfigRegistry::clear()
{
    d->items.clear();
    d->index.clear();
}

int ConfigRegistry::itemCount() const
{
    return static_cast<int>(d->items.size());
}

const ConfigItem* ConfigRegistry::itemAt(int index) const
{
    if (index < 0 || static_cast<size_t>(index) >= d->items.size())
        return nullptr;
    return &d->items[static_cast<size_t>(index)];
}

const ConfigItem* ConfigRegistry::item(const String& key) const
{
    auto it = d->index.find(key);
    if (it == d->index.end())
        return nullptr;
    return &d->items[it->second];
}

const std::vector<ConfigItem>& ConfigRegistry::items() const
{
    return d->items;
}

std::vector<String> ConfigRegistry::groups() const
{
    std::vector<String> out;
    for (const auto& item : d->items) {
        bool found = false;
        for (const auto& g : out) {
            if (g == item.group()) {
                found = true;
                break;
            }
        }
        if (!found)
            out.push_back(item.group());
    }
    return out;
}

V_APPFW_NS_END
