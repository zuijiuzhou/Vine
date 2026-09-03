#include <vine/graphics/RenderEngine.hpp>

#include <vine/graphics/Camera.hpp>
#include <vine/graphics/CameraManipulator.hpp>
#include <vine/graphics/Overlay.hpp>
#include <vine/graphics/RenderBackend.hpp>
#include <vine/graphics/RenderPass.hpp>
#include <vine/graphics/RenderTarget.hpp>
#include <vine/graphics/Scene.hpp>
#include <vine/window/InputEvent.hpp>

#include <algorithm>

V_GRAPHICS_NS_BEGIN

V_OBJECT_META_IMPL(RenderEngine, vine::Object);

RenderEngine::RenderEngine()
{
    // No implicit pipeline: the engine starts empty. The caller registers the
    // scene passes explicitly (addPass()) or through a RenderPipelineBuilder,
    // and may set a default content scene (setScene) and a master camera
    // (setMasterCamera).
}

RenderEngine::~RenderEngine()
{
    shutdown();
}

raw_ptr<RenderBackend> RenderEngine::backend() const
{
    return backend_.get();
}

void RenderEngine::setBackend(intrusive_ptr<RenderBackend> backend)
{
    // Re-assigning the same backend is a no-op (idempotent).
    if (backend_ == backend) {
        return;
    }
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
    // Forward the shading preset before the backend builds its shader sets.
    backend_->setShaderPreset(shader_preset_);
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
    frame_ctx_.dt = dt;
    backend_->beginFrame();

    // Fresh named-output registry per frame: every producer publishes during
    // the ordered pass run, so a consumer only ever samples this frame's
    // output and stale entries from removed producers disappear automatically.
    outputs_.clear();

    // Scene-pass pipeline in ascending order: negative orders run first
    // (shadow / depth / g-buffer pre-pass), the window-present pass (master
    // camera, null render target) conventionally sits at order 0, and
    // positive orders run after (post-processing / compositing). The pipeline
    // is exactly what the caller registered - the engine auto-registers
    // nothing. Each pass resolves its declared inputs just before it runs and
    // publishes its named output right after.
    for (const auto& slot : passes_) {
        // Effective content: the pass's explicit binding, else the default
        // content scene (scene_). A pass whose effective content is null
        // decides for itself: the base RenderPass draws nothing, while
        // content-agnostic passes (e.g. ScreenPass) still run and use their
        // resolved inputs.
        const raw_ptr<Scene> content = (slot.content != nullptr) ? slot.content.get() : scene_.get();
        resolvePassInputs(slot.pass.get());
        drawScenePass(slot.pass.get(), content);
        publishPassOutput(slot.pass.get());
    }

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

const FrameContext& RenderEngine::frameContext() const
{
    return frame_ctx_;
}

void RenderEngine::addOverlay(intrusive_ptr<Overlay> overlay)
{
    if (overlay == nullptr) {
        return;
    }
    // Registering the same overlay instance twice is ignored: it must only be
    // drawn once per frame.
    for (const auto& existing : overlays_) {
        if (existing == overlay) {
            return;
        }
    }
    overlays_.push_back(std::move(overlay));
}

bool RenderEngine::isEnginePass(raw_ptr<RenderPass> pass) const
{
    if (pass == nullptr) {
        return false;
    }
    for (const auto& slot : passes_) {
        if (slot.pass.get() == pass) {
            return true;
        }
    }
    return false;
}

bool RenderEngine::passUsedByOverlay(raw_ptr<RenderPass> pass, raw_ptr<Overlay> except) const
{
    if (pass == nullptr) {
        return false;
    }
    for (const auto& overlay : overlays_) {
        if (overlay == nullptr || overlay.get() == except) {
            continue;
        }
        if (overlay->pass() == pass) {
            return true;
        }
    }
    return false;
}

bool RenderEngine::passStillUsedElsewhere(raw_ptr<RenderPass> pass, raw_ptr<Overlay> except) const
{
    return isEnginePass(pass) || passUsedByOverlay(pass, except);
}

void RenderEngine::removeOverlay(raw_ptr<Overlay> overlay)
{
    // An overlay draws through its pass, whose camera identifies the overlay's
    // backend view slot (VsgRenderer keys non-main views by pass->camera()).
    // Release that view and the pass's render target ONLY when nothing else
    // still draws the same pass after this overlay goes away; otherwise the
    // surviving user would keep drawing a torn-down view / target.
    if (backend_ != nullptr && overlay != nullptr) {
        RenderPass* pass = overlay->pass();
        if (pass != nullptr && !passStillUsedElsewhere(pass, overlay)) {
            backend_->releaseOverlay(pass->camera());
            backend_->releaseRenderTarget(pass->renderTarget());
        }
    }
    overlays_.erase(std::remove_if(overlays_.begin(), overlays_.end(),
                                   [overlay](const intrusive_ptr<Overlay>& p) {
                                       return p.get() == overlay;
                                   }),
                    overlays_.end());
}

void RenderEngine::clearOverlays()
{
    // Snapshot + dedupe the overlay passes: after the clear only the engine's
    // own scene-pass pipeline can still draw them, so release every unique
    // pass the engine does not keep (isEnginePass).
    std::vector<raw_ptr<RenderPass>> passes;
    for (const auto& overlay : overlays_) {
        if (overlay != nullptr && overlay->pass() != nullptr) {
            passes.push_back(overlay->pass());
        }
    }
    std::sort(passes.begin(), passes.end());
    passes.erase(std::unique(passes.begin(), passes.end()), passes.end());

    if (backend_ != nullptr) {
        for (raw_ptr<RenderPass> pass : passes) {
            if (!isEnginePass(pass)) {
                backend_->releaseOverlay(pass->camera());
                backend_->releaseRenderTarget(pass->renderTarget());
            }
        }
    }
    overlays_.clear();
}

void RenderEngine::addPass(intrusive_ptr<RenderPass> pass, int order)
{
    addPass(std::move(pass), nullptr, order);
}

void RenderEngine::addPass(intrusive_ptr<RenderPass> pass, intrusive_ptr<Scene> content, int order)
{
    if (pass == nullptr) {
        return;
    }
    // Registering the same pass instance twice is ignored: it would otherwise
    // run twice per frame. To rebind content use bindPassContent(); to change
    // the order remove the pass and re-add it.
    for (const auto& slot : passes_) {
        if (slot.pass.get() == pass.get()) {
            return;
        }
    }
    // Keep the pipeline ascending by order; equal orders preserve insertion
    // order (stable), so ties are resolved by the addPass() call sequence.
    const auto it = std::find_if(passes_.begin(), passes_.end(),
                                 [order](const PassSlot& slot) { return slot.order > order; });
    passes_.insert(it, PassSlot{ std::move(pass), std::move(content), order });
}

void RenderEngine::removePass(raw_ptr<RenderPass> pass)
{
    // Remove the pass from the ordered pass list first: only afterwards is it
    // known whether anything else still draws it.
    passes_.erase(std::remove_if(passes_.begin(), passes_.end(),
                                 [pass](const PassSlot& slot) {
                                     return slot.pass.get() == pass;
                                 }),
                  passes_.end());

    // The pass has left the engine pass pipeline; only an overlay can still
    // draw it. Release its backend resources (view keyed by the pass's camera
    // + any off-screen target) only when no overlay uses it - an overlay-owned
    // pass is released when that overlay is removed.
    if (backend_ != nullptr && pass != nullptr && !passUsedByOverlay(pass, nullptr)) {
        backend_->releaseOverlay(pass->camera());
        backend_->releaseRenderTarget(pass->renderTarget());
    }
}

void RenderEngine::clearPasses()
{
    // Snapshot + dedupe the registered passes: after the clear only overlays
    // can still draw them, so release every unique pass no overlay uses.
    std::vector<raw_ptr<RenderPass>> passes;
    for (const auto& slot : passes_) {
        if (slot.pass != nullptr) {
            passes.push_back(slot.pass.get());
        }
    }
    std::sort(passes.begin(), passes.end());
    passes.erase(std::unique(passes.begin(), passes.end()), passes.end());

    if (backend_ != nullptr) {
        for (raw_ptr<RenderPass> pass : passes) {
            if (!passUsedByOverlay(pass, nullptr)) {
                backend_->releaseOverlay(pass->camera());
                backend_->releaseRenderTarget(pass->renderTarget());
            }
        }
    }
    passes_.clear();
}

std::size_t RenderEngine::passCount() const
{
    return passes_.size();
}

void RenderEngine::bindPassContent(raw_ptr<RenderPass> pass, intrusive_ptr<Scene> content)
{
    for (auto& slot : passes_) {
        if (slot.pass.get() == pass) {
            slot.content = std::move(content);
            return;
        }
    }
}

raw_ptr<Scene> RenderEngine::contentOf(raw_ptr<RenderPass> pass) const
{
    for (const auto& slot : passes_) {
        if (slot.pass.get() == pass) {
            return (slot.content != nullptr) ? slot.content.get() : scene_.get();
        }
    }
    return nullptr;
}

void RenderEngine::drawScenePass(raw_ptr<RenderPass> pass, raw_ptr<Scene> content)
{
    // The pass decides what a null content means: the base RenderPass draws
    // nothing (execute returns when the scene is null), while content-agnostic
    // passes such as ScreenPass still execute against their resolved inputs.
    if (pass != nullptr) {
        pass->execute(content, backend_.get());
    }
}

void RenderEngine::resolvePassInputs(raw_ptr<RenderPass> pass)
{
    if (pass == nullptr) {
        return;
    }
    const auto& names = pass->inputNames();
    if (names.empty()) {
        return;
    }
    std::vector<raw_ptr<RenderTarget>> resolved;
    resolved.reserve(names.size());
    for (const auto& name : names) {
        resolved.push_back(resolve(name));
    }
    pass->resolveInputTextures(resolved);
}

void RenderEngine::publishPassOutput(raw_ptr<RenderPass> pass)
{
    if (pass == nullptr) {
        return;
    }
    const String&    name   = pass->outputName();
    raw_ptr<RenderTarget> target = pass->renderTarget();
    if (name.empty() || target == nullptr) {
        return;
    }
    publish(name, intrusive_ptr<RenderTarget>(target));
}

void RenderEngine::publish(const String& name, intrusive_ptr<RenderTarget> target)
{
    if (name.empty() || target == nullptr) {
        return;
    }
    outputs_[name] = std::move(target);
}

raw_ptr<RenderTarget> RenderEngine::resolve(const String& name) const
{
    const auto it = outputs_.find(name);
    return (it != outputs_.end()) ? it->second.get() : nullptr;
}

void RenderEngine::unpublish(const String& name)
{
    outputs_.erase(name);
}

void RenderEngine::setScene(intrusive_ptr<Scene> scene)
{
    // Setting the same scene again is a no-op (idempotent).
    if (scene_ == scene) {
        return;
    }
    scene_ = std::move(scene);
}

raw_ptr<Scene> RenderEngine::scene() const
{
    return scene_.get();
}

void RenderEngine::setMasterCamera(intrusive_ptr<Camera> camera)
{
    // Setting the same camera again is a no-op (idempotent). The engine does
    // not propagate the camera to any pass: registered passes keep their own
    // cameras, and the window-present pass is registered with this camera.
    if (master_camera_ == camera) {
        return;
    }
    master_camera_ = std::move(camera);

}

raw_ptr<Camera> RenderEngine::masterCamera() const
{
    return master_camera_.get();
}

void RenderEngine::setShaderPreset(ShaderPreset preset)
{
    shader_preset_ = preset;
}

ShaderPreset RenderEngine::shaderPreset() const
{
    return shader_preset_;
}

void RenderEngine::setCameraManipulator(intrusive_ptr<CameraManipulator> manipulator)
{
    // Setting the same manipulator again is a no-op (idempotent).
    if (camera_manipulator_ == manipulator) {
        return;
    }
    camera_manipulator_ = std::move(manipulator);
}

raw_ptr<CameraManipulator> RenderEngine::cameraManipulator() const
{
    return camera_manipulator_.get();
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
    frame_ctx_.surface_width  = event.width;
    frame_ctx_.surface_height = event.height;
    // Refresh anything the viewport size feeds into, e.g. the camera aspect.
    if (camera_manipulator_ != nullptr) {
        camera_manipulator_->onResize(event);
    } else if (master_camera_ != nullptr
               && master_camera_->projectionType() == vine::graphics::Camera::ProjectionType::Perspective) {
        // No manipulator owns the view: keep the master camera's projection
        // aspect in step with the surface so content is not stretched when the
        // window changes shape.
        const double aspect = static_cast<double>(event.width) / static_cast<double>(event.height);
        master_camera_->setProjectionMatrixAsPerspective(master_camera_->fieldOfView(), aspect,
                                                         master_camera_->nearPlane(), master_camera_->farPlane());
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
