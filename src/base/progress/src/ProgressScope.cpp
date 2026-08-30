#include <vine/progress/ProgressScope.hpp>

#include <vine/progress/ProgressIndicator.hpp>
#include <vine/progress/ProgressRange.hpp>

V_PROGRESS_NS_BEGIN

ProgressScope::ProgressScope() = default;

ProgressScope::ProgressScope(ProgressIndicator* indicator)
  : indicator_(indicator)
  , active_(indicator != nullptr)
{}

ProgressScope::ProgressScope(const ProgressRange& range, const std::string& name, double max)
  : indicator_(range.state_ != nullptr && range.state_->parent_scope_ != nullptr
                   ? range.state_->parent_scope_->indicator()
                   : nullptr)
  , parent_(range.state_ != nullptr ? range.state_->parent_scope_ : nullptr)
  , name_(name)
  , start_(range.state_ != nullptr ? range.state_->start_ : 0.0)
  , global_length_(range.state_ != nullptr ? range.state_->delta_ : 0.0)
  , local_length_(max > 1e-6 ? max : 1e-6)
  , active_(indicator_ != nullptr && range.state_ != nullptr && !range.state_->completed_)
{
    // The scope reports this portion now; the range handles are disarmed.
    if (range.state_ != nullptr) {
        range.state_->completed_ = true;
    }
}

ProgressScope::~ProgressScope()
{
    complete();
}

ProgressRange ProgressScope::next(double step)
{
    if (!active_ || step <= 0.0) {
        return ProgressRange();
    }

    const double current = localToGlobal(local_pos_);
    const double next    = localToGlobal(local_pos_ + step);
    local_pos_ += step;

    const double delta = next - current;
    if (delta <= 0.0) {
        return ProgressRange();
    }

    return ProgressRange(*this, start_ + current, delta);
}

bool ProgressScope::isCancelled() const
{
    return indicator_ != nullptr && indicator_->isCancelled();
}

bool ProgressScope::isActive() const
{
    return active_;
}

double ProgressScope::localPos() const
{
    if (!active_) {
        return local_length_;
    }

    const double global = indicator_->position() - start_;
    if (global <= 0.0) {
        return 0.0;
    }

    const double distance = global_length_ - global;
    if (distance <= 1e-6) {
        return local_length_;
    }

    return local_length_ * global / global_length_;
}

double ProgressScope::localLength() const
{
    return local_length_;
}

double ProgressScope::globalLength() const
{
    return global_length_;
}

const std::string& ProgressScope::name() const
{
    return name_;
}

const ProgressScope* ProgressScope::parent() const
{
    return parent_;
}

ProgressIndicator* ProgressScope::indicator() const
{
    return indicator_;
}

void ProgressScope::complete()
{
    if (!active_) {
        return;
    }

    const double current = localToGlobal(local_pos_);
    local_pos_ = local_length_;

    const double delta = global_length_ - current;
    if (delta > 0.0) {
        indicator_->increment(delta);
    }

    active_ = false;
}

double ProgressScope::localToGlobal(double value) const
{
    if (value <= 0.0) {
        return 0.0;
    }

    if (local_length_ - value <= 1e-6) {
        return global_length_;
    }

    return global_length_ * value / local_length_;
}

V_PROGRESS_NS_END
