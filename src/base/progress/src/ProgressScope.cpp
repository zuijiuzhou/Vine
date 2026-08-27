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
  : indicator_(range.parent_scope_ != nullptr ? range.parent_scope_->indicator_ : nullptr)
  , parent_(range.parent_scope_)
  , name_(name)
  , start_(range.start_)
  , portion_(range.delta_)
  , max_(max > 1e-6 ? max : 1e-6)
  , active_(indicator_ != nullptr && !range.used_)
{
    range.used_ = true;
}

ProgressScope::~ProgressScope()
{
    close();
}

ProgressRange ProgressScope::next(double step)
{
    if (!active_ || step <= 0.0) {
        return ProgressRange();
    }

    const double current = localToGlobal(value_);
    const double next    = localToGlobal(value_ + step);
    value_ += step;

    const double delta = next - current;
    if (delta <= 0.0) {
        return ProgressRange();
    }

    return ProgressRange(*this, start_ + current, delta);
}

bool ProgressScope::more() const
{
    return !isCanceled();
}

bool ProgressScope::isCanceled() const
{
    return indicator_ != nullptr && indicator_->isCanceled();
}

bool ProgressScope::isActive() const
{
    return active_;
}

double ProgressScope::value() const
{
    if (!active_) {
        return max_;
    }

    const double global = indicator_->position() - start_;
    if (global <= 0.0) {
        return 0.0;
    }

    const double distance = portion_ - global;
    if (distance <= 1e-6) {
        return max_;
    }

    return max_ * global / portion_;
}

double ProgressScope::maxValue() const
{
    return max_;
}

double ProgressScope::portion() const
{
    return portion_;
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

void ProgressScope::close()
{
    if (!active_) {
        return;
    }

    const double current = localToGlobal(value_);
    value_ = max_;

    const double delta = portion_ - current;
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

    if (max_ - value <= 1e-6) {
        return portion_;
    }

    return portion_ * value / max_;
}

V_PROGRESS_NS_END
