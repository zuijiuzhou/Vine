#pragma once
#include "graphics_global.hpp"

#include "Ray.hpp"

#include <vine/intrusive_ptr.hpp>
#include <vine/Object.hpp>
#include <vine/RefCounted.hpp>
#include <vine/String.hpp>
#include <vine/math/Matrix4x4.hpp>
#include <vine/math/Vector2.hpp>
#include <vine/math/Vector3.hpp>

V_GRAPHICS_NS_BEGIN

using vine::math::Vec3d;
using vine::math::Vec2d;
using vine::math::Mat4d;

/**
 * @brief Camera class managing view and projection matrices.
 *
 * Camera stores complete view and projection matrices. The view matrix is set
 * at once with setViewMatrixAsLookAt(); the projection matrix is set at once
 * with setProjectionMatrixAsPerspective() or setProjectionMatrixAsOrtho().
 * This avoids intermediate inconsistent states and mirrors the OSG camera API.
 */
class V_GRAPHICS_API Camera : public Object, public RefCounted<Camera> {
    V_OBJECT_META_DECL;

  public:
    enum class ProjectionType {
        Perspective,    ///< Perspective projection.
        Orthographic,   ///< Orthographic projection.
    };

  public:
    Camera();
    ~Camera();

  public:
    /** @brief Gets the camera name. */
    String name() const;

    /** @brief Sets the camera name. */
    void setName(const String& name);

    /** @brief Gets the current projection type. */
    ProjectionType projectionType() const;

    /** @brief Sets the view matrix from a look-at specification.
     *
     * @param eye    Camera eye position (world space).
     * @param center Point the camera looks at (world space).
     * @param up     Up vector (does not need to be orthogonal).
     */
    void setViewMatrixAsLookAt(const Vec3d& eye, const Vec3d& center, const Vec3d& up);

    /** @brief Sets the projection matrix from perspective parameters.
     *
     * @param fovy    Vertical field of view in degrees.
     * @param aspect  Aspect ratio (width / height).
     * @param zNear   Near clipping plane distance (> 0).
     * @param zFar    Far clipping plane distance.
     */
    void setProjectionMatrixAsPerspective(double fovy, double aspect, double zNear, double zFar);

    /** @brief Sets the projection matrix from orthographic parameters.
     *
     * @param left   Left clipping plane.
     * @param right  Right clipping plane.
     * @param bottom Bottom clipping plane.
     * @param top    Top clipping plane.
     * @param zNear  Near clipping plane distance.
     * @param zFar   Far clipping plane distance.
     */
    void setProjectionMatrixAsOrtho(double left, double right, double bottom, double top,
                                    double zNear, double zFar);

    /** @brief Gets the camera eye position (last set via setViewMatrixAsLookAt). */
    Vec3d eye() const;

    /** @brief Gets the camera target/center point (last set via setViewMatrixAsLookAt). */
    Vec3d target() const;

    /** @brief Gets the up vector (last set via setViewMatrixAsLookAt). */
    Vec3d up() const;

    /** @brief Gets the near clipping plane distance. */
    double nearPlane() const;

    /** @brief Gets the far clipping plane distance. */
    double farPlane() const;

    /** @brief Gets the field of view in degrees (perspective only). */
    double fieldOfView() const;

    /** @brief Gets the aspect ratio (width / height). */
    double aspectRatio() const;

    /** @brief Gets the orthographic view height (top - bottom). */
    double orthographicHeight() const;

    /** @brief Gets the view matrix. */
    Mat4d viewMatrix() const;

    /** @brief Gets the projection matrix. */
    Mat4d projectionMatrix() const;

    /** @brief Converts screen coordinates to a world-space picking ray.
     *
     * @param screenPos Normalized screen coordinates in [0, 1]
     *                  (pixel / viewport dimension).
     * @return Ray in world space.
     */
    Ray screenToWorldRay(const Vec2d& screenPos) const;

  private:
    String name_;
    Mat4d view_{ Mat4d() };
    Mat4d projection_{ Mat4d() };
    ProjectionType projection_type_ = ProjectionType::Perspective;
    // Copy of the last look-at parameters, used for ray generation.
    Vec3d eye_{ 0.0, 0.0, 5.0 };
    Vec3d center_{ 0.0, 0.0, 0.0 };
    Vec3d up_{ 0.0, 1.0, 0.0 };
    // Copy of the last projection parameters, used for queries.
    double near_plane_ = 0.1;
    double far_plane_ = 1000.0;
    double fov_ = 45.0;
    double aspect_ratio_ = 1.0;
    double ortho_height_ = 10.0;
};

using CameraPtr = intrusive_ptr<Camera>;

V_GRAPHICS_NS_END
