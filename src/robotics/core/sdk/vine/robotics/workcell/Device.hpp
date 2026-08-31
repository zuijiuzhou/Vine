#pragma once

#include <vine/robotics/robot_core_global.hpp>

#include <cstddef>
#include <memory>
#include <vector>

#include <vine/String.hpp>
#include <vine/math/Isometry3.hpp>
#include <vine/robotics/kinematics/Frame.hpp>
#include <vine/robotics/kinematics/Q.hpp>
#include <vine/robotics/kinematics/State.hpp>
#include <vine/robotics/workcell/Joint.hpp>
#include <vine/robotics/workcell/Link.hpp>
#include <vine/robotics/workcell/SceneObject.hpp>

V_ROBOTICS_WORKCELL_NS_BEGIN

/**
 * @brief Base class of all devices in a workcell.
 *
 * A device is a scene-managed object composed of links and joints (held by
 * smart pointers) plus multiple end frames. It inherits its base frame from
 * SceneObject, and every frame it carries (base + joints + ends) is
 * registered in the SceneObject frame registry as a raw pointer. Devices may
 * be mounted on other devices (e.g. a scanner on a robot's TCP) through a
 * parent link; the world pose then cascades down the parent chain, so a
 * child follows the parent's motion automatically.
 *
 * Ownership: the Workcell owns every device (unique ownership). The parent
 * reference is a non-owning raw pointer; a parent must belong to the same
 * Workcell and must outlive its children (the Workcell detaches children when
 * a device is removed).
 */
class Device : public SceneObject {
  public:
    /**
     * @brief Constructs a device with the given name.
     *
     * @param name The device name.
     */
    explicit Device(const String& name)
      : name_(name)
    {}

    /**
     * @brief Destroys the device.
     */
    ~Device() override = default;

    /**
     * @brief Returns the object kind.
     *
     * @return SceneObjectKind::Device.
     */
    SceneObjectKind kind() const override
    {
        return SceneObjectKind::Device;
    }

    /**
     * @brief Returns the device name.
     *
     * @return The name.
     */
    const String& name() const override
    {
        return name_;
    }

    /**
     * @brief Returns the model name (e.g. the robot model id).
     *
     * @return The model name.
     */
    const String& modelName() const
    {
        return model_name_;
    }

    /**
     * @brief Sets the model name (e.g. the robot model id).
     *
     * @param model_name The new model name.
     */
    void setModelName(const String& model_name)
    {
        model_name_ = model_name;
    }

    /**
     * @brief Returns whether the device is fully configured and usable.
     *
     * @return true when the device carries links or joints.
     */
    bool isValid() const
    {
        return !links_.empty() && !joints_.empty();
    }

    /**
     * @brief Returns the frame of the parent this device mounts to.
     *
     * Empty means the parent's root frame; for a device with links it can be
     * a link name or an end frame name.
     *
     * @return The parent frame.
     */
    kinematics::Frame* parentFrame() const
    {
        return base_frame_->parent();
    }

    /**
     * @brief Returns the device's joint values from a scene state.
     *
     * @param state The scene state.
     * @return The joint values, empty when the device is not registered in
     *         the state.
     */
    kinematics::Q getQ(const kinematics::State& state) const;

    /**
     * @brief Writes the device's joint values into a scene state.
     *
     * @param q The joint values; its size must match the device dof.
     * @param state The scene state to write into.
     * @throws std::invalid_argument when q.size() differs from the device
     *         dof.
     */
    void setQ(const kinematics::Q& q, kinematics::State& state);

    /**
     * @brief Returns the links of the kinematic chain.
     *
     * @return The links.
     */
    std::vector<std::unique_ptr<Link>>& links()
    {
        return links_;
    }

    /**
     * @brief Returns the links of the kinematic chain.
     *
     * @return The links.
     */
    const std::vector<std::unique_ptr<Link>>& links() const
    {
        return links_;
    }

    /**
     * @brief Returns the joints connecting the links.
     *
     * @return The joints.
     */
    std::vector<std::unique_ptr<Joint>>& joints()
    {
        return joints_;
    }

    /**
     * @brief Returns the joints connecting the links.
     *
     * @return The joints.
     */
    const std::vector<std::unique_ptr<Joint>>& joints() const
    {
        return joints_;
    }

    /**
     * @brief Returns the number of end frames.
     *
     * @return The end count.
     */
    std::size_t getNumEnds() const
    {
        return ends_.size();
    }

    /**
     * @brief Returns all end frames.
     *
     * @return The end frames.
     */
    std::vector<kinematics::Frame*> getEndFrames() const
    {
        std::vector<kinematics::Frame*> ends;
        ends.reserve(ends_.size());
        for (const auto& end : ends_) {
            ends.push_back(end.get());
        }
        return ends;
    }

    /**
     * @brief Returns the end frame at the given index.
     *
     * @param index The end index.
     * @return The end frame, or null when out of range.
     */
    kinematics::Frame* getEndFrame(std::size_t index) const
    {
        return index < ends_.size() ? ends_[index].get() : nullptr;
    }

  private:
    String                                          name_;
    String                                          model_name_;
    math::Isometry3d                                local_pose_;
    std::vector<std::unique_ptr<Link>>              links_;
    std::vector<std::unique_ptr<Joint>>             joints_;
    std::vector<std::unique_ptr<kinematics::Frame>> ends_;
};

V_ROBOTICS_WORKCELL_NS_END
