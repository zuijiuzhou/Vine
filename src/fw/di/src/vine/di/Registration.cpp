#include <vine/di/Registration.hpp>

#include <vine/Exception.hpp>
#include <vine/Ptr.hpp>

V_DI_NS_BEGIN

Registration::Registration(Type type)
{
    V_CHECK_NULL_THROW(type);
    service_type_ = type;
}

Registration& Registration::instance(RefObject* inst)
{
    if (inst && !inst->isKindOf(service_type_)) {
        throw vine::Exception(vine::Exception::INVALID_ARGUMENTS, u8"The 'inst' is not kind of the service type.");
    }
    inst_     = inst;
    lifetime_ = Lifetime::Singleton;
    return *this;
}

V_DI_NS_END
