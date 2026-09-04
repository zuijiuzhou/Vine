#include <gtest/gtest.h>

#include <vine/appfw/AppBuilder.hpp>
#include <vine/appfw/Application.hpp>
#include <vine/appfw/PluginManager.hpp>
#include <vine/graphics/RenderBackend.hpp>
#include <vine/graphics/RenderBackendRegistry.hpp>

#include <memory>

namespace
{

/**
 * @brief Boots an Application the same way app/src/main.cpp does, but headless.
 *
 * Mirrors the real startup: build the application through the builder, then
 * load all plugins. PluginManager discovers gfx_backend_vsg from the default
 * plugin directory; GfxBackendVsgPlugin::load() registers the "vsg" backend
 * into RenderBackendRegistry.
 *
 * The Application is created exactly once per test suite: QCoreApplication is
 * a Qt global singleton, so a second Application in the same process would
 * collide when init() constructs a new QCoreApplication.
 */
std::unique_ptr<vine::appfw::Application> bootApplication()
{
    static char arg0[] = "test_vsg";
    static char* argv[] = { arg0, nullptr };

    vine::appfw::AppConfig config;
    config.name = "Vine";
    auto app = vine::appfw::createApplication(config, 1, argv);
    EXPECT_NE(app, nullptr);
    if (app != nullptr) {
        app->pluginManager()->loadAll();
    }
    return app;
}

class VsgBackendPluginTest : public ::testing::Test
{
  protected:
    static void SetUpTestSuite()
    {
        s_app = bootApplication();
    }

    static std::unique_ptr<vine::appfw::Application> s_app;
};

std::unique_ptr<vine::appfw::Application> VsgBackendPluginTest::s_app;

}  // namespace

TEST_F(VsgBackendPluginTest, PluginRegistersVsgBackend)
{
    ASSERT_NE(s_app, nullptr);

    auto& registry = vine::graphics::RenderBackendRegistry::instance();
    EXPECT_TRUE(registry.has(u8"vsg"))
        << "gfx_backend_vsg plugin should have registered the 'vsg' backend";
}

TEST_F(VsgBackendPluginTest, CreateBackendByName)
{
    ASSERT_NE(s_app, nullptr);

    auto& registry = vine::graphics::RenderBackendRegistry::instance();

    // The plugin path must produce a VsgRenderer instance. initialize() is
    // intentionally not called here: it creates a real Vulkan window/device,
    // which is unsuitable for a headless unit test (would block waiting for a
    // GPU surface). Backend init/rendering is exercised by the real app.
    auto backend = registry.create(u8"vsg");
    ASSERT_NE(backend, nullptr);
}
