#pragma once

#include <vine/RawPtr.hpp>
#include <vine/appfw/ConfigManager.hpp>
#include <vine/appfw/ConfigRegistry.hpp>

#include <vine/appfw/gui/Window.hpp>

V_APPFWGUI_NS_BEGIN

/**
 * @brief Configuration window: renders editors from the registry (ConfigRegistry)
 * and reads/writes values through ConfigManager.
 *
 * Each item gets an editor by type (String -> QLineEdit, Bool -> QCheckBox,
 * Int -> QSpinBox, Double -> QDoubleSpinBox, Choice -> QComboBox), shown in a
 * two-level "category -> group" layout (ConfigCategory -> ConfigGroup).
 * Edits write back to ConfigManager immediately (its changed event fires);
 * refresh() reloads values from storage and reset() restores defaults.
 * Inherits Window; show() non-modally or exec() modally.
 */
class V_APPFW_API ConfigWindow : public Window {
    V_OBJECT_META_DECL

  public:
    /**
     * @brief Builds the window content from the registry item tree, using config
     * as the data source.
     *
     * @param registry Registry holding the item tree.
     * @param config   Config manager holding the values.
     */
    ConfigWindow(ConfigRegistry* registry, ConfigManager* config);
    ~ConfigWindow() override;

  public:
    /**
     * @brief Reloads all editor values from ConfigManager.
     */
    void refresh();
    /**
     * @brief Restores defaults: writes the default value where present,
     * otherwise removes the key.
     */
    void reset();

    /**
     * @brief The associated registry.
     */
    RawPtr<ConfigRegistry> registry() const;
    /**
     * @brief The associated config manager.
     */
    RawPtr<ConfigManager> config() const;

  private:
    struct Impl;
    Impl*       dptr();
    const Impl* dptr() const;
};

V_APPFWGUI_NS_END
