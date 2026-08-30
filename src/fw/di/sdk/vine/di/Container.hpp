#pragma once
#include "di_global.hpp"
#include <vine/RefObject.hpp>

V_DI_NS_BEGIN

class Registration;

V_DECLARE_PIMPL(Container)
V_DEFINE_PTR(Container)

/**
 * @brief Dependency injection container.
 *
 * Holds service registrations keyed by their runtime Type and resolves them
 * on demand. Instances come from a pre-set instance, an instance factory, or
 * a chained impl-type registration. A singleton is cached after its first
 * creation; a transient instance is re-created on every resolve call.
 *
 * The container is Qt-free and depends only on the core runtime type system
 * (Type / RefObject / SPtr). resolve() returns a non-owning raw pointer:
 * the container keeps ownership of pre-set and singleton instances, while a
 * transient instance must be adopted by the caller through SPtr.
 */
class V_DI_API Container : public RefObject {
    V_OBJECT_META_DECL

  public:
    /**
     * @brief Creates an empty container.
     */
    Container();

    /**
     * @brief Destroys the container and releases the singleton instances it owns.
     */
    ~Container();

  public:
    /**
     * @brief Registers a service descriptor.
     *
     * @param reg Registration to add; it must provide an instance, a factory or an impl type.
     * @throws vine::Exception with INVALID_ARGUMENTS when the registration cannot create a service.
     * @throws vine::Exception with ITEM_ALREADY_EXISTS when the service type is already registered.
     */
    void add(const Registration& reg);

    /**
     * @brief Resolves the service registered for the given type.
     *
     * @param type Runtime type of the requested service.
     * @return The resolved service instance, or nullptr when the type is not
     *         registered or cannot be created.
     */
    RefObject* resolve(Type type) const;

    /**
     * @brief Resolves the service registered for T.
     *
     * @tparam T Service type; must derive from RefObject.
     * @return The resolved service instance cast to T*, or nullptr.
     */
    template <RefObjectBased T>
    T* resolve() const;

  private:
    struct Impl;
    Impl* const d;
};

template <RefObjectBased T>
T* Container::resolve() const
{
    return resolve(T::desc());
}

V_DI_NS_END
