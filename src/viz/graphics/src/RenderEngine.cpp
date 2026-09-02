#include <vine/graphics/RenderEngine.hpp>

#include <vine/graphics/Camera.hpp>
#include <vine/graphics/CameraManipulator.hpp>
#include <vine/graphics/Overlay.hpp>
#include <vine/graphics/RenderBackend.hpp>
#include <vine/graphics/RenderPass.hpp>
#include <vine/graphics/Scene.hpp>
#include <vine/window/InputEvent.hpp>

#include <algorithm>

V_GRAPHICS_NS_BEGIN

V_OBJECT_META_IMPL(RenderEngine, vine::Object);

RenderEngine::RenderEngine()
{
    scene_ = intrusive_ptr<Scene>(new Scene());
    camera_ = intrusive_ptr<Camera>(new Camera());
    main_pass_ = intrusive_ptr<RenderPass>(new RenderPass());
    main_pass_->setCamera(camera_.get());
}

RenderEngine::~RenderEngine()
{
    shutdown();
}

RenderBackend* RenderEngine::backend() const
{
    return backend_.get();
}

void RenderEngine::setBackend(intrusive_ptr<RenderBackend> backend)
{
    backend_ = std::move(backend);
}

bool RenderEngine::initialize()
{
    if (backend_ == nullptr) {
        return false;
    }
    if (native_handle_ != nullptr) {
        backend_->setWindowHandle(native_handle_);
    }
    initialized_ = backend_->initialize();
    if (initialized_) {
        // Pre-frame warm-up: execute every visible overlay pass once so the
        // backend builds and compiles overlay content before the first frame
        // is submitted. Compiling content that is added mid-frame (i.e. when
        // an overlay slot is first encountered inside frame()) has proven
        // unreliable in the vsg backend; the main content is pre-compiled
        // during backend initialize, and overlays need the same treatment.
        // Re-execution in later frames is a no-op for already-built content.
        for (const auto& overlay : overlays_) {
            if (overlay == nullptr || !overlay->visible()) {
                continue;
            }
            RenderPass* pass = overlay->pass();
            Scene* content = overlay->content();
            if (pass == nullptr || content == nullptr) {
                continue;
            }
            overlay->update(0.0);
            pass->execute(content, backend_.get());
        }
    }
    return initialized_;
}

void RenderEngine::shutdown()
{
    if (backend_ != nullptr && initialized_) {
        backend_->shutdown();
    }
    initialized_ = false;
}

void RenderEngine::frame(double dt)
{
    if (!initialized_ || backend_ == nullptr) {
        return;
    }
    backend_->beginFrame();
    main_pass_->execute(scene_.get(), backend_.get());

    // Draw registered overlays in ascending zOrder on top of the main scene.
    std::stable_sort(overlays_.begin(), overlays_.end(),
                     [](const intrusive_ptr<Overlay>& a, const intrusive_ptr<Overlay>& b) {
                         return a->zOrder() < b->zOrder();
                     });
    for (const auto& overlay : overlays_) {
        if (overlay == nullptr || !overlay->visible()) {
            continue;
        }
        RenderPass* pass = overlay->pass();
        Scene* content = overlay->content();
        if (pass == nullptr || content == nullptr) {
            continue;
        }
        overlay->update(dt);
        pass->execute(content, backend_.get());
    }

    backend_->endFrame();
    backend_->swapBuffers();
}

void RenderEngine::addOverlay(intrusive_ptr<Overlay> overlay)
{
    if (overlay != nullptr) {
        overlays_.push_back(std::move(overlay));
    }
}

void RenderEngine::removeOverlay(Overlay* overlay)
{
    overlays_.erase(std::remove_if(overlays_.begin(), overlays_.end(),
                                   [overlay](const intrusive_ptr<Overlay>& p) {
                                       return p.get() == overlay;
                                   }),
                    overlays_.end());
}

void RenderEngine::clearOverlays()
{
    overlays_.clear();
}

void RenderEngine::setScene(Scene* scene)
{
    scene_ = scene;
}

Scene* RenderEngine::scene() const
{
    return scene_.get();
}

void RenderEngine::setCamera(Camera* camera)
{
    camera_ = camera;
    main_pass_->setCamera(camera);
}

Camera* RenderEngine::camera() const
{
    return camera_.get();
}

void RenderEngine::setMainPass(RenderPass* pass)
{
    main_pass_ = pass;
}

RenderPass* RenderEngine::mainPass() const
{
    return main_pass_.get();
}

void RenderEngine::setCameraManipulator(CameraManipulator* manipulator)
{
    camera_manipulator_ = manipulator;
}

CameraManipulator* RenderEngine::cameraManipulator() const
{
    return camera_manipulator_;
}

void RenderEngine::pushEvent(const vine::window::MouseEvent& event)
{
    if (camera_manipulator_ == nullptr) {
        return;
    }
    if (event.button == vine::window::MouseButton::None) {
        camera_manipulator_->onMouseMove(event);
    } else if (event.pressed) {
        camera_manipulator_->onMousePress(event);
    } else {
        camera_manipulator_->onMouseRelease(event);
    }
}

void RenderEngine::pushEvent(const vine::window::ScrollEvent& event)
{
    if (camera_manipulator_ != nullptr) {
        camera_manipulator_->onScroll(event);
    }
}

void RenderEngine::pushEvent(const vine::window::KeyEvent& event)
{
    if (camera_manipulator_ == nullptr) {
        return;
    }
    if (event.pressed) {
        camera_manipulator_->onKeyDown(event);
    } else {
        camera_manipulator_->onKeyUp(event);
    }
}

void RenderEngine::pushEvent(const vine::window::ResizeEvent& event)
{
    if (event.width <= 0 || event.height <= 0) {
        return;
    }
    // Refresh anything the viewport size feeds into, e.g. the camera aspect.
    if (camera_manipulator_ != nullptr) {
        camera_manipulator_->onResize(event);
    } else if (camera_ != nullptr
               && camera_->projectionType() == vine::graphics::Camera::ProjectionType::Perspective) {
        // No manipulator owns the view: keep the main camera's projection
        // aspect in step with the surface so content is not stretched when the
        // window changes shape.
        const double aspect = static_cast<double>(event.width) / static_cast<double>(event.height);
        camera_->setProjectionMatrixAsPerspective(camera_->fieldOfView(), aspect,
                                                  camera_->nearPlane(), camera_->farPlane());
    }
    if (initialized_ && backend_ != nullptr) {
        backend_->resize(event.width, event.height);
    }
    for (const auto& overlay : overlays_) {
        if (overlay != nullptr) {
            overlay->onSurfaceResized(event.width, event.height);
        }
    }
}

void RenderEngine::setWindowHandle(void* native_handle)
{
    native_handle_ = native_handle;
}

V_GRAPHICS_NS_END
