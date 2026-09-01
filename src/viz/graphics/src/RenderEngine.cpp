#include <vine/graphics/RenderEngine.hpp>

#include <vine/graphics/Camera.hpp>
#include <vine/graphics/RenderBackend.hpp>
#include <vine/graphics/RenderPass.hpp>
#include <vine/graphics/Scene.hpp>

V_GRAPHICS_NS_BEGIN

V_OBJECT_META_IMPL(RenderEngine, vine::Object);

struct RenderEngine::Data {
    RenderBackend* backend = nullptr;
    intrusive_ptr<Scene> scene;
    intrusive_ptr<Camera> camera;
    intrusive_ptr<RenderPass> main_pass;
    bool initialized = false;
};

RenderEngine::RenderEngine(RenderBackend* backend)
  : d(new Data())
{
    d->backend = backend;
    d->scene = intrusive_ptr<Scene>(new Scene());
    d->camera = intrusive_ptr<Camera>(new Camera());
    d->main_pass = intrusive_ptr<RenderPass>(new RenderPass());
    d->main_pass->setCamera(d->camera.get());
}

RenderEngine::~RenderEngine()
{
    shutdown();
    delete d;
}

RenderBackend* RenderEngine::backend() const
{
    return d->backend;
}

bool RenderEngine::initialize()
{
    if (d->backend == nullptr) {
        return false;
    }
    d->initialized = d->backend->initialize();
    return d->initialized;
}

void RenderEngine::shutdown()
{
    if (d->backend != nullptr && d->initialized) {
        d->backend->shutdown();
    }
    d->initialized = false;
}

void RenderEngine::frame()
{
    if (!d->initialized || d->backend == nullptr) {
        return;
    }
    d->backend->beginFrame();
    d->main_pass->execute(d->scene.get(), d->backend);
    d->backend->endFrame();
    d->backend->swapBuffers();
}

void RenderEngine::setScene(Scene* scene)
{
    d->scene = scene;
}

Scene* RenderEngine::scene() const
{
    return d->scene.get();
}

void RenderEngine::setCamera(Camera* camera)
{
    d->camera = camera;
    d->main_pass->setCamera(camera);
}

Camera* RenderEngine::camera() const
{
    return d->camera.get();
}

void RenderEngine::setMainPass(RenderPass* pass)
{
    d->main_pass = pass;
}

RenderPass* RenderEngine::mainPass() const
{
    return d->main_pass.get();
}

V_GRAPHICS_NS_END
