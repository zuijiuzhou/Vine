#include <vine/progress/ProgressIndicator.hpp>

#include <vine/progress/ProgressRange.hpp>
#include <vine/progress/ProgressScope.hpp>

V_PROGRESS_NS_BEGIN

ProgressIndicator::ProgressIndicator()
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
    position_           = 0.0;
    root_scope_->value_ = 0.0;
    cancelled_.store(false);
    return root_scope_->next();
}

double ProgressIndicator::position() const
{
    return position_;
}

bool ProgressIndicator::isCancelled() const
{
    return cancelled_.load();
}

void ProgressIndicator::cancel()
{
    cancelled_.store(true);
}

void ProgressIndicator::increment(double step)
{
    std::lock_guard<std::mutex> lock(mutex_);

    position_ += step;
    if (position_ > 1.0) {
        position_ = 1.0;
    }
}

V_PROGRESS_NS_END
