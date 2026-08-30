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
    // Reject an instance that is not derived from the service type.
    if (inst && !inst->isKindOf(service_type_)) {
        throw vine::Exception(vine::Exception::INVALID_ARGUMENTS, u8"The 'inst' is not kind of the service type.");
    }
    inst_     = inst;                // The container takes ownership through SPtr.
    lifetime_ = Lifetime::Singleton; // A pre-set instance is inherently shared.
    return *this;
}

V_DI_NS_END
