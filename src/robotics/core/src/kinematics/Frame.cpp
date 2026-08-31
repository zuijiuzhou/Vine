#include <vine/robotics/kinematics/Frame.hpp>

#include <algorithm>
#include <stdexcept>

V_ROBOTICS_KINEMATICS_NS_BEGIN

Frame::Frame() = default;

Frame::Frame(FrameType type)
  : type_(type)
{}

void Frame::addChild(Frame* child)
{
    if (!child) {
        throw std::invalid_argument("Frame::addChild, child is null");
    }
    if (child == this) {
        throw std::invalid_argument("Frame::addChild, cannot add self as child");
    }
    if (child->parent_) {
        throw std::invalid_argument("Frame::addChild, child already has a parent");
    }
    child->parent_ = this;
    children_.push_back(child);
}

void Frame::removeChild(Frame* child)
{
    if (!child) {
        throw std::invalid_argument("Frame::removeChild, child is null");
    }
    if (child == this) {
        throw std::invalid_argument("Frame::removeChild, cannot remove self");
    }
    const auto it = std::find(children_.begin(), children_.end(), child);
    if (it == children_.end()) {
        throw std::invalid_argument("Frame::removeChild, child is not a direct child");
    }
    children_.erase(it);
    child->parent_ = nullptr;
}

V_ROBOTICS_KINEMATICS_NS_END