// #pragma once

// #include <vine/robotics/robot_core_global.hpp>

// #include <memory>
// #include <vector>

// #include <vine/String.hpp>
// #include <vine/math/Isometry3.hpp>
// #include <vine/robotics/kinematics/State.hpp>
// #include <vine/robotics/workcell/SceneObject.hpp>

// V_ROBOTICS_WORKCELL_NS_BEGIN

// class Device;

// /**
//  * @brief A robotic workcell: the container of all scene objects.
//  *
//  * The Workcell OWNS every top-level scene object (robot, scanner, part,
//  * table, fence, ...) with unique ownership, and resolves world poses across
//  * device mount chains.
//  *
//  * Ownership rules:
//  *  - addSceneObject() transfers unique ownership to the workcell and returns
//  *    a non-owning handle for configuration.
//  *  - Device parent references are non-owning; removing a device detaches any
//  *    child that referenced it as a parent, so no dangling parent remains.
//  *  - Links are internal to MotionDevice and never owned here.
//  */
// class Workcell
// {
//   public:
//     /**
//      * @brief Constructs an empty workcell.
//      */
//     Workcell() = default;

//     /**
//      * @brief Destroys the workcell and all owned objects.
//      */
//     ~Workcell() = default;

//     Workcell(const Workcell&)            = delete;
//     Workcell& operator=(const Workcell&) = delete;

//     /**
//      * @brief Returns the workcell name.
//      *
//      * @return The name.
//      */
//     const String& name() const
//     {
//         return name_;
//     }

//     /**
//      * @brief Sets the workcell name.
//      *
//      * @param name The new name.
//      */
//     void setName(const String& name)
//     {
//         name_ = name;
//     }

//     /**
//      * @brief Adds a scene object, taking ownership.
//      *
//      * @param object The object to add.
//      * @return A non-owning pointer to the added object, or nullptr.
//      */
//     SceneObject* addSceneObject(std::unique_ptr<SceneObject> object);

//     /**
//      * @brief Finds a scene object by name.
//      *
//      * @param name The object name.
//      * @return The object, or nullptr when not found.
//      */
//     SceneObject* findSceneObject(const String& name) const;

//     /**
//      * @brief Removes and destroys a scene object by name.
//      *
//      * Any device that referenced the removed object as a parent is detached.
//      *
//      * @param name The object name.
//      * @return true when an object was removed.
//      */
//     bool removeSceneObject(const String& name);

//     /**
//      * @brief Returns all owned scene objects (non-owning handles).
//      *
//      * @return The scene objects.
//      */
//     std::vector<SceneObject*> sceneObjects() const;
 
//   private:
//     String                                       name_;
//     std::vector<std::unique_ptr<SceneObject>>    objects_;
// };

// V_ROBOTICS_WORKCELL_NS_END
