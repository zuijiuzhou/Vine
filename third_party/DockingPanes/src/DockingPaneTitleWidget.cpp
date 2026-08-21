/*
 * This file is part of DockingPanes. (https://github.com/KestrelRadarSensors/dockingpanes)
 *
 * (C) 2020 Kestrel Radar Sensors (https://www.kestrelradarsensors.com)
 *
 * DockingPanes is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DockingPanes is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with DockingPanes.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <QApplication>
#include <QDebug>
#include <QFont>
#include <QMouseEvent>
#include <QPainter>

#include "DockingPaneTheme.h"
#include "DockingPaneTitleWidget.h"

DockingPaneTitleWidget::DockingPaneTitleWidget(QString text, QWidget* parent)
  : QWidget(parent)
  , m_text(text)
  , m_active(false)
{
    this->setFont(QFont("Segoe UI", 9));
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(qApp, &QApplication::focusChanged, this, &DockingPaneTitleWidget::onFocusChanged);
}

DockingPaneTitleWidget::~DockingPaneTitleWidget()
{
    // 析构期间(如父窗口销毁子控件)焦点可能变化, focusChanged 会调用到基类析构已开始的
    // 本对象导致 assertObjectType 断言, 因此提前断开连接
    disconnect(qApp, &QApplication::focusChanged, this, &DockingPaneTitleWidget::onFocusChanged);
}

void DockingPaneTitleWidget::resizeEvent(QResizeEvent*)
{
    this->setMinimumHeight(6 + this->fontMetrics().height());
    this->setMaximumHeight(6 + this->fontMetrics().height());
}

void DockingPaneTitleWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    QRect    drawnRect;

    int leftMargin, rightMargin;

    leftMargin  = 5;
    rightMargin = 5;

    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::Antialiasing, true);

    QFontMetrics fm(this->font());

    QString elidedText = fm.elidedText(m_text, Qt::ElideRight, width() - 6);

    p.setPen(DockingPaneTheme::titleTextColor(m_active));

    p.drawText(leftMargin, 0, width() - (leftMargin + rightMargin), height(), Qt::AlignVCenter, elidedText, &drawnRect);

    if ((width() - drawnRect.width() - (leftMargin + rightMargin * 2)) > 0) {
        drawPattern(&p, drawnRect.width() + (leftMargin * 2), 0, width() - drawnRect.width() - ((leftMargin * 2) + rightMargin), height());
    }
}

void DockingPaneTitleWidget::setText(QString text)
{
    m_text = text;
}

void DockingPaneTitleWidget::drawPattern(QPainter* p, int x, int y, int w, int h)
{
    QPixmap pixMap(4, 5);

    pixMap.fill(Qt::transparent);

    QPainter pp(&pixMap);

    pp.setPen(DockingPaneTheme::titlePatternColor(m_active));

    pp.drawPoint(0, 0);
    pp.drawPoint(0, 4);
    pp.drawPoint(2, 2);

    p->setBrushOrigin(x, ((y + h) / 2) - 2);
    p->fillRect(x, ((y + h) / 2) - 2, w, 5, QBrush(pixMap));
}

void DockingPaneTitleWidget::mouseMoveEvent(QMouseEvent* event)
{
    Q_EMIT titleBarMoved(this->mapToGlobal(event->pos()));
}

void DockingPaneTitleWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        if (m_grabbing) {
            releaseMouse();
            m_grabbing = false;
        }

        Q_EMIT titleBarEndMove(this->mapToGlobal(event->pos()));
        event->accept();
    }
}

void DockingPaneTitleWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        this->setFocus();

        // Keep receiving mouse events for the whole drag, even when the
        // cursor moves over the top-level docking indicator overlays.
        grabMouse();
        m_grabbing = true;

        event->accept();
        Q_EMIT titleBarStartMove(this->mapToGlobal(event->pos()));
    }
}

void DockingPaneTitleWidget::reacquireGrab(void)
{
    // The grab is lost when the owning window is recreated (docked pane is
    // floated mid-drag); grab the new window to keep the drag alive.
    if (m_grabbing) {
        grabMouse();
    }
}

void DockingPaneTitleWidget::takeGrab(void)
{
    // 飞窗拖出转浮动时飞窗被隐藏、抓取释放; 由本标题接管鼠标以继续拖动
    if (!m_grabbing) {
        grabMouse();
        m_grabbing = true;
    }
}

void DockingPaneTitleWidget::setActive(bool active)
{
    m_active = active;
    update();
}

void DockingPaneTitleWidget::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::PaletteChange || event->type() == QEvent::ApplicationPaletteChange) {
        update();
    }

    QWidget::changeEvent(event);
}

void DockingPaneTitleWidget::onFocusChanged(QWidget*, QWidget* now)
{
    setActive(this->isAncestorOf(now));
}
