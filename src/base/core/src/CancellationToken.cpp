#include <vine/CancellationToken.hpp>

V_CORE_NS_BEGIN

CancellationToken::CancellationToken()
  : state_(std::make_shared<std::atomic<bool>>(false))
{}

void CancellationToken::cancel() noexcept
{
    state_->store(true, std::memory_order_relaxed);
}

bool CancellationToken::isCancellationRequested() const noexcept
{
    return state_->load(std::memory_order_relaxed);
}

void CancellationToken::reset() noexcept
{
    state_->store(false, std::memory_order_relaxed);
}

V_CORE_NS_END
