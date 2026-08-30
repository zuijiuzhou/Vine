#include <vine/di/Registration.hpp>

#include <vine/Exception.hpp>
#include <vine/IntrusivePtr.hpp>

V_DI_NS_BEGIN

Registration::Registration(TypeId type)
{
    V_CHECK_NULL_THROW(type);
    service_type_ = type;
}

Registration& Registration::instance(ServiceBase* inst)
{
    // Reject an instance that is not derived from the service type.
    if (inst && !inst->isKindOf(service_type_)) {
        throw vine::Exception(vine::Exception::INVALID_ARGUMENTS, u8"The 'inst' is not kind of the service type.");
    }
    inst_     = inst;                        // The container takes ownership through IntrusivePtr.
    lifetime_ = Lifetime::Singleton; // A pre-set instance is inherently shared.
    return *this;
}

V_DI_NS_END
