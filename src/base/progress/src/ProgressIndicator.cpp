#include <vine/progress/ProgressIndicator.hpp>

#include <utility>

#include <vine/progress/ProgressRange.hpp>
#include <vine/progress/ProgressScope.hpp>

V_PROGRESS_NS_BEGIN

ProgressIndicator::ProgressIndicator(std::stop_token token)
  : token_(std::move(token))
{
    root_scope_ = new ProgressScope(this);
}

ProgressIndicator::~ProgressIndicator()
{
    // Disarm the root scope so its destructor does not call increment() on a
    // partially destroyed indicator.
    root_scope_->indicator_ = nullptr;
    root_scope_->active_    = false;
    delete root_scope_;
}

ProgressRange ProgressIndicator::start()
{
    position_.store(0.0, std::memory_order_relaxed);
    root_scope_->local_pos_ = 0.0;

    return root_scope_->next();
}

double ProgressIndicator::position() const
{
    return position_.load(std::memory_order_relaxed);
}

bool ProgressIndicator::isCancelled() const
{
    return token_.stop_requested();
}

std::stop_token ProgressIndicator::token() const
{
    return token_;
}

void ProgressIndicator::increment(double step)
{
    double current = position_.load(std::memory_order_relaxed);
    for (;;) {
        const double next = (current > 1.0 - step) ? 1.0 : current + step;
        if (position_.compare_exchange_weak(current, next, std::memory_order_relaxed)) {
            return;
        }
    }
}

V_PROGRESS_NS_END
