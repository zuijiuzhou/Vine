#include <vine/appfw/ConfigRegistry.hpp>

#include <algorithm>
#include <memory>
#include <utility>

V_APPFW_NS_BEGIN

struct ConfigRegistry::Impl {
    std::vector<std::unique_ptr<ConfigCategory>> categories;
};

ConfigRegistry::ConfigRegistry()
  : d(new Impl)
{}

ConfigRegistry::~ConfigRegistry()
{
    delete d;
}

ConfigCategory* ConfigRegistry::addCategory(String name)
{
    if (category(name) != nullptr)
        return nullptr; // Duplicate name, reject
    d->categories.push_back(std::unique_ptr<ConfigCategory>(new ConfigCategory(std::move(name), this)));
    return d->categories.back().get();
}

bool ConfigRegistry::removeCategory(const String& name)
{
    for (auto it = d->categories.begin(); it != d->categories.end(); ++it) {
        if ((*it)->name() == name) {
            d->categories.erase(it);
            return true;
        }
    }
    return false;
}

void ConfigRegistry::clear()
{
    d->categories.clear();
}

std::vector<ConfigCategory*> ConfigRegistry::categories() const
{
    std::vector<ConfigCategory*> out;
    out.reserve(d->categories.size());
    for (const auto& c : d->categories) out.push_back(c.get());
    std::stable_sort(out.begin(), out.end(), [](const ConfigCategory* a, const ConfigCategory* b) { return a->order() < b->order(); });
    return out;
}

ConfigCategory* ConfigRegistry::category(const String& name) const
{
    for (const auto& c : d->categories) {
        if (c->name() == name)
            return c.get();
    }
    return nullptr;
}

int ConfigRegistry::itemCount() const
{
    int n = 0;
    for (const auto& c : d->categories) {
        for (ConfigGroup* g : c->groups()) n += static_cast<int>(g->items().size());
    }
    return n;
}

const ConfigItem* ConfigRegistry::item(const String& key) const
{
    for (const auto& c : d->categories) {
        for (ConfigGroup* g : c->groups()) {
            if (const ConfigItem* it = g->item(key))
                return it;
        }
    }
    return nullptr;
}

bool ConfigRegistry::removeItem(const String& key)
{
    for (const auto& c : d->categories) {
        for (ConfigGroup* g : c->groups()) {
            if (g->item(key) != nullptr)
                return g->removeItem(key);
        }
    }
    return false;
}

V_APPFW_NS_END
