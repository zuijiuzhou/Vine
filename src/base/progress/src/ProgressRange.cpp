#include <vine/progress/ProgressRange.hpp>

#include <vine/progress/ProgressIndicator.hpp>
#include <vine/progress/ProgressScope.hpp>

V_PROGRESS_NS_BEGIN

struct ProgressRange::State {
    const ProgressScope* parent_scope_{nullptr};

    double start_{0.0};

    double delta_{0.0};

    bool completed_{false};

    ~State()
    {
        report();
    }

    void report()
    {
        if (completed_ || parent_scope_ == nullptr || parent_scope_->indicator() == nullptr) {
            return;
        }

        completed_ = true;
        parent_scope_->indicator()->increment(delta_);
    }
};

ProgressRange::ProgressRange() = default;

ProgressRange::ProgressRange(const ProgressScope& parent, double start, double delta)
  : state_(std::make_shared<State>())
{
    state_->parent_scope_ = &parent;
    state_->start_        = start;
    state_->delta_        = delta;
}

bool ProgressRange::isActive() const
{
    return state_ != nullptr && !state_->completed_ && state_->parent_scope_ != nullptr
        && state_->parent_scope_->indicator() != nullptr;
}

bool ProgressRange::isCancelled() const
{
    return state_ != nullptr && state_->parent_scope_ != nullptr
        && state_->parent_scope_->indicator() != nullptr && state_->parent_scope_->indicator()->isCancelled();
}

void ProgressRange::complete()
{
    if (state_ != nullptr) {
        state_->report();
    }
}

V_PROGRESS_NS_END
