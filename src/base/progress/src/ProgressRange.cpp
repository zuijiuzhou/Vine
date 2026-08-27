#include <vine/progress/ProgressRange.hpp>

#include <vine/progress/ProgressIndicator.hpp>
#include <vine/progress/ProgressScope.hpp>

V_PROGRESS_NS_BEGIN

ProgressRange::ProgressRange() = default;

ProgressRange::ProgressRange(const ProgressRange& other)
  : parent_scope_(other.parent_scope_)
  , start_(other.start_)
  , delta_(other.delta_)
  , used_(other.used_)
{
    other.used_ = true;
}

ProgressRange& ProgressRange::operator=(const ProgressRange& other)
{
    if (this != &other) {
        parent_scope_ = other.parent_scope_;
        start_        = other.start_;
        delta_        = other.delta_;
        used_         = other.used_;
        other.used_   = true;
    }
    return *this;
}

ProgressRange::ProgressRange(const ProgressScope& parent, double start, double delta)
  : parent_scope_(&parent)
  , start_(start)
  , delta_(delta)
{}

ProgressRange::~ProgressRange()
{
    close();
}

bool ProgressRange::isActive() const
{
    return !used_ && parent_scope_ != nullptr && parent_scope_->indicator_ != nullptr;
}

bool ProgressRange::isCanceled() const
{
    return parent_scope_ != nullptr && parent_scope_->indicator_ != nullptr && parent_scope_->indicator_->isCanceled();
}

bool ProgressRange::more() const
{
    return !isCanceled();
}

void ProgressRange::close()
{
    if (!isActive()) {
        return;
    }

    parent_scope_->indicator_->increment(delta_);
    parent_scope_ = nullptr;
    used_         = true;
}

V_PROGRESS_NS_END
