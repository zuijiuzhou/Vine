#include <vine/RefObject.hpp>

V_CORE_NS_BEGIN

V_OBJECT_META_IMPL(RefObject, Object)

RefObject::RefObject() noexcept
{
    cb_ = new PtrControlBlock();
    // The object holds an implicit weak reference to the control block
    // so that the control block can outlive the object until all weak
    // references are released. Initialize weak_refs to 1 for that owner.
    cb_->weak_refs.store(1, std::memory_order_relaxed);
}

RefObject::~RefObject() noexcept
{
    // When the object is destroyed (by any path), release the implicit
    // weak reference the object holds to the control block. If that was
    // the last weak reference and there are no strong references,
    // delete the control block.
    if (cb_) {
        // Defensive: if someone manually `delete`d the object (misuse),
        // ensure the control block is marked as having no strong owners
        // so it can be reclaimed when weak refs drop to zero. Note this
        // cannot fix all misuse cases (e.g. other outstanding strong
        // references lead to UB); it's a mitigation to avoid leaking
        // the control block on manual deletion.
        cb_->strong_refs.store(0, std::memory_order_release);

        if (cb_->weak_refs.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            if (cb_->strong_refs.load(std::memory_order_acquire) == 0) {
                std::atomic_thread_fence(std::memory_order_acquire);
                delete cb_;
            }
        }
    }
}

// Note: strong/weak ref management moved to RefPtr/WRefPtr in Ptr.hpp.

V_CORE_NS_END
