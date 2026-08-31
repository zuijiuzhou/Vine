#include <vine/robotics/workcell/Device.hpp>

#include <stdexcept>

V_ROBOTICS_WORKCELL_NS_BEGIN
 

kinematics::Q Device::getQ(const kinematics::State& state) const
{
    return {};
}

void Device::setQ(const kinematics::Q& q, kinematics::State& state)
{
    
}

V_ROBOTICS_WORKCELL_NS_END
