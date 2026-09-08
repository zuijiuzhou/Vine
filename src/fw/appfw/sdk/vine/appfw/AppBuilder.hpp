#pragma once

#include "appfw_global.hpp"

#include <filesystem>
#include <memory>
#include <string>

V_APPFW_NS_BEGIN

class Application;

/**
 * @brief Declarative configuration used to build an application.
 *
 * The factory functions apply the settings to the process-wide application:
 * name becomes the Qt application name, plugin_dir becomes the plugin search
 * directory. language is a placeholder reserved for locale selection.
 */
struct AppConfig {
    /// Application name, applied as QCoreApplication::applicationName().
    std::string name;

    /// Plugin search directory; empty keeps the default.
    std::filesystem::path plugin_dir;

    /// Locale placeholder reserved for i18n (not wired yet).
    std::string language;
};

/**
 * @brief Builds a headless application from config.
 *
 * Applies the plugin directory and application name, then initializes the
 * application.
 *
 * @param config Application configuration.
 * @param argc Command line argument count.
 * @param argv Command line arguments.
 * @return The initialized application.
 */
V_APPFW_API std::unique_ptr<Application> createApplication(const AppConfig& config, int argc, char** argv);

V_APPFW_NS_END
