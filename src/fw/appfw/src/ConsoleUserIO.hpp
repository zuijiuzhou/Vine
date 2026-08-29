#pragma once

#include <vine/appfw/UserIO.hpp>

V_APPFW_NS_BEGIN

/**
 * @brief Headless UserIO: writes to stdout and reads from stdin.
 *
 * Each getXxxAsync() prints the prompt to stdout and blocks on std::getline
 * until a line is read. Intended for console applications without a GUI event
 * loop.
 */
class ConsoleUserIO : public UserIO {
    V_OBJECT_META_DECL;
    V_DISABLE_COPY_MOVE(ConsoleUserIO);

  public:
    ConsoleUserIO();

  public:
    void putString(const String& str) override;
    void clear() override;

    vine::co::Task<std::optional<String>>        getStringAsync(const String& prompt = {}) override;
    vine::co::Task<std::optional<int8_t>>        getIntAsync(const String& prompt = {}) override;
    vine::co::Task<std::optional<double>>        getDoubleAsync(const String& prompt = {}) override;
    vine::co::Task<std::optional<math::Point3d>> getPoint3dAsync(const String& prompt = {}) override;
};

V_APPFW_NS_END
