#include <vine/appfw/gui/ProgressPresenter.hpp>

#include <chrono>

#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QTimer>

#include <vine/appfw/gui/UIElementData.hpp>
#include <vine/progress/ProgressHost.hpp>

V_APPFWGUI_NS_BEGIN

namespace
{

/// Poll period for the ambient host.
constexpr int kPollIntervalMs = 100;

/// How long an operation runs before the bar appears.
constexpr int kShowDelayMs = 400;

/// How long the bar stays visible after the operation ends.
constexpr int kHideDelayMs = 300;

} // namespace

V_OBJECT_META_IMPL(ProgressPresenter, Control)

struct ProgressPresenter::Impl : public UIElementData {
    /// Owning presenter, used by the poll timer to drive onTick().
    ProgressPresenter* self = nullptr;

    QProgressBar* bar             = nullptr;
    QLabel*       chain_label     = nullptr;
    QLabel*       background_label = nullptr;
    QPushButton*  cancel          = nullptr;
    QTimer*       timer           = nullptr;

    /// The tracked foreground host (drives the main bar), or nullptr.
    vine::progress::ProgressHost* foreground = nullptr;
    std::chrono::steady_clock::time_point host_seen_at{};
    std::chrono::steady_clock::time_point hide_at{};
    bool bar_visible   = false;
    bool progress_seen = false;
    bool hide_pending  = false;
};

ProgressPresenter::ProgressPresenter(QWidget* parent)
  : Control(new Impl(), new QWidget(parent))
{
    auto* data = dptr();
    auto* root = impl<QWidget>();
    data->self = this;

    auto* layout = new QHBoxLayout(root);
    layout->setContentsMargins(4, 0, 4, 0);
    layout->setSpacing(4);

    data->bar = new QProgressBar(root);
    data->bar->setRange(0, 1000);
    data->bar->setFixedWidth(160);
    data->bar->setTextVisible(false);
    layout->addWidget(data->bar);

    data->chain_label = new QLabel(root);
    data->chain_label->setVisible(false);
    layout->addWidget(data->chain_label);

    data->background_label = new QLabel(root);
    data->background_label->setVisible(false);
    layout->addWidget(data->background_label);

    data->cancel = new QPushButton(QStringLiteral("Cancel"), root);
    data->cancel->setFixedHeight(data->bar->sizeHint().height());
    layout->addWidget(data->cancel);

    // The wrapper is not a QObject, so signal contexts use the native widget.
    // The cancel button stops the foreground operation only.
    QObject::connect(data->cancel, &QPushButton::clicked, root, [data] {
        if (data->foreground) {
            data->foreground->cancelSource().request_stop();
        }
    });

    data->timer = new QTimer(root);
    QObject::connect(data->timer, &QTimer::timeout, root, [data] { data->self->onTick(); });
    data->timer->start(kPollIntervalMs);

    root->setVisible(false);
}

ProgressPresenter::~ProgressPresenter()
{
    // d is deleted by UIElement
}

bool ProgressPresenter::isBusy() const
{
    // Any active host (foreground or background) keeps the presenter engaged.
    return vine::progress::ProgressHost::isActive();
}

void ProgressPresenter::onTick()
{
    using namespace std::chrono;

    auto* const data  = dptr();
    const auto  now   = steady_clock::now();
    auto* const fg    = vine::progress::ProgressHost::current();
    const auto  hosts = vine::progress::ProgressHost::activeHosts();
    const auto  chain = vine::progress::ProgressHost::foregroundStack();
    // 后台宿主 = 活跃宿主中不在前台栈里的（真正并行的任务）。
    const std::size_t bg_count = hosts.size() >= chain.size() ? hosts.size() - chain.size() : 0;

    // Track the current foreground (top of stack); reset per-host state on
    // change. When a nested child takes over the bar, keep it visible instead
    // of flickering.
    if (fg != data->foreground) {
        data->foreground = fg;
        if (fg != nullptr) {
            data->host_seen_at  = now;
            data->progress_seen = false;
        }
        else {
            data->bar_visible = false;
        }
    }

    // Foreground main bar (innermost LongRunning command).
    if (fg != nullptr) {
        if (fg->indicator().position() > 0.0) {
            data->progress_seen = true;
        }
        if (!data->bar_visible) {
            const auto elapsed = duration_cast<milliseconds>(now - data->host_seen_at).count();
            if (elapsed >= kShowDelayMs) {
                data->bar_visible = true;
            }
        }
        if (data->bar_visible) {
            if (data->progress_seen) {
                const double pos = fg->indicator().position();
                data->bar->setRange(0, 1000);
                data->bar->setValue(static_cast<int>(pos * 1000));
                data->bar->setTextVisible(true);
                if (fg->label().empty()) {
                    data->bar->setFormat(QStringLiteral("%p%"));
                }
                else {
                    data->bar->setFormat(QString::fromStdString(fg->label()) + QStringLiteral(" %p%"));
                }
            }
            else {
                // No progress reported yet: show an indeterminate busy bar.
                data->bar->setRange(0, 0);
                data->bar->setTextVisible(false);
            }
        }
    }

    // Chain breadcrumb (only meaningful while a nested child is running).
    const bool has_chain = chain.size() > 1;
    data->chain_label->setVisible(has_chain);
    if (has_chain) {
        QString text;
        for (auto* h : chain) {
            if (!text.isEmpty()) {
                text += QStringLiteral(" \u25B8 ");
            }
            QString name = QString::fromStdString(h->label());
            text += name.isEmpty() ? QStringLiteral("\u2026") : name;
        }
        data->chain_label->setText(text);
    }

    // Background activity badge (parallel tasks running alongside).
    const bool has_bg = bg_count > 0;
    data->background_label->setVisible(has_bg);
    if (has_bg) {
        data->background_label->setText(QStringLiteral("后台 %1").arg(static_cast<int>(bg_count)));
    }

    // Overall visibility: foreground bar past the threshold, or background
    // activity; hide shortly after everything drains.
    const bool want_show = data->bar_visible || has_bg;
    if (want_show) {
        data->hide_pending = false;
        setVisible(true);
    }
    else {
        if (!data->hide_pending) {
            data->hide_pending = true;
            data->hide_at      = now + milliseconds(kHideDelayMs);
        }
        if (now >= data->hide_at) {
            setVisible(false);
        }
    }
}

inline auto ProgressPresenter::dptr() -> Impl*
{
    return static_cast<Impl*>(UIElement::d);
}

inline auto ProgressPresenter::dptr() const -> const Impl*
{
    return static_cast<const Impl*>(UIElement::d);
}

V_APPFWGUI_NS_END
