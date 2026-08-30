#pragma once

#include "appfw_global.hpp"

#include <vector>

#include <vine/String.hpp>

#include "ConfigCategory.hpp"

V_APPFW_NS_BEGIN

/**
 * @brief Configuration registry: plugins build a "category -> group -> item"
 * display tree here.
 *
 * Stores metadata only, not values (values live in ConfigManager); the tree
 * structure is the display order and it also provides whole-tree queries.
 * Qt-free.
 */
class V_APPFW_API ConfigRegistry {
  public:
    ConfigRegistry();
    ~ConfigRegistry();

    ConfigRegistry(const ConfigRegistry&)            = delete;
    ConfigRegistry& operator=(const ConfigRegistry&) = delete;

    /**
     * @brief Adds a category.
     *
     * @param name Unique category name.
     * @return The new category, or nullptr if a category with the same name exists.
     */
    ConfigCategory* addCategory(String name);
    /**
     * @brief Removes the category with the given name.
     *
     * @param name Category name.
     * @return true if removed, false if not found.
     */
    bool removeCategory(const String& name);
    /**
     * @brief Clears the whole tree.
     */
    void clear();

    /**
     * @brief All categories (display order, stable-sorted by order).
     */
    std::vector<ConfigCategory*> categories() const;
    /**
     * @brief Looks up a category by name.
     *
     * @param name Category name.
     * @return The category, or nullptr if not found.
     */
    ConfigCategory* category(const String& name) const;

    /**
     * @brief Total number of items (sum over all groups in the tree).
     */
    int itemCount() const;
    /**
     * @brief Looks up an item by key across the whole tree.
     *
     * @param key Item key.
     * @return The item, or nullptr if not found.
     */
    const ConfigItem* item(const String& key) const;
    /**
     * @brief Removes the item with the given key across the whole tree.
     *
     * @param key Item key.
     * @return true if removed, false if not found.
     */
    bool removeItem(const String& key);

  private:
    struct Impl;
    Impl* const d;
};

V_APPFW_NS_END
