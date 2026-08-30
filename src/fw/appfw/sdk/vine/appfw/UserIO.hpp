#pragma once
#include "appfw_global.hpp"

#include <cstdint>
#include <optional>

#include <vine/RefObject.hpp>
#include <vine/String.hpp>
#include <vine/async/Task.hpp>
#include <vine/math/Point3.hpp>

V_APPFW_NS_BEGIN

class CommandManager;

class V_APPFW_API UserIO : public Object {
    V_OBJECT_META_DECL;

  public:
    UserIO();

  public:
    virtual void putString(const String& str) = 0;

    /**
     * @brief Clears all previously written output.
     *
     * The base implementation does nothing; visual implementations clear their
     * console panel and headless implementations clear the terminal.
     */
    virtual void clear();

    /**
     * @brief Asynchronously requests a string from the user.
     *
     * @param prompt Prompt text shown to the user.
     * @return A task yielding the entered string, or std::nullopt if cancelled.
     */
    virtual vine::async::Task<std::optional<String>> getStringAsync(const String& prompt = {}) = 0;

    /**
     * @brief Asynchronously requests an integer from the user.
     *
     * @param prompt Prompt text shown to the user.
     * @return A task yielding the entered value, or std::nullopt if cancelled.
     */
    virtual vine::async::Task<std::optional<int8_t>> getIntAsync(const String& prompt = {}) = 0;

    /**
     * @brief Asynchronously requests a double from the user.
     *
     * @param prompt Prompt text shown to the user.
     * @return A task yielding the entered value, or std::nullopt if cancelled.
     */
    virtual vine::async::Task<std::optional<double>> getDoubleAsync(const String& prompt = {}) = 0;

    /**
     * @brief Asynchronously requests a 3D point from the user.
     *
     * @param prompt Prompt text shown to the user.
     * @return A task yielding the picked point, or std::nullopt if cancelled.
     */
    virtual vine::async::Task<std::optional<math::Point3d>> getPoint3dAsync(const String& prompt = {}) = 0;

    /**
     * @brief Sets the command manager that idle input is dispatched to.
     *
     * @param manager Command manager, or nullptr to unbind.
     */
    virtual void setCommandManager(CommandManager* manager);

    /**
     * @brief Returns the bound command manager.
     *
     * @return The command manager, or nullptr if unbound.
     */
    CommandManager* commandManager() const;

  private:
    CommandManager* command_manager_{ nullptr };
};

using UserIOPtr = SPtr<UserIO>;

V_APPFW_NS_END
