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

#ifndef DOCKAUTOHIDEBUTTON_H
#define DOCKAUTOHIDEBUTTON_H

#include <QPushButton>

#include "DockingPaneBase.h"
#include "DockingPaneContainer.h"

class QStyleOptionButton;
class QTimer;

/**
 * @brief Button on the auto-hide strip (shows the pane name; can be laid out
 * horizontally or vertically).
 *
 * Sits on the auto-hide strip at the edge of the main window and points to a
 * pinned pane; clicking it opens the corresponding flyout via DockingPaneManager.
 */
class DockAutoHideButton : public QPushButton {
    Q_OBJECT

  public:
    /**
     * @brief Edge the button is located on (determines text layout direction and swap).
     */
    enum Position
    {
        Left,
        Right,
        Top,
        Bottom
    };

  public:
    explicit DockAutoHideButton(Position pos, QWidget* parent = nullptr);
    explicit DockAutoHideButton(const QString& text, QWidget* parent = nullptr);
    DockAutoHideButton(const QIcon& icon, const QString& text, QWidget* parent = nullptr);

  public:
    /**
     * @brief Text layout orientation (Horizontal/Vertical).
     */
    Qt::Orientation orientation() const;
    /**
     * @brief Sets the text layout orientation (Horizontal/Vertical).
     */
    void setOrientation(Qt::Orientation orientation);

    bool mirrored() const;
    void setMirrored(bool mirrored);

    /**
     * @brief Switches the text/color-strip drawing direction (inside/outside the edge).
     */
    void swapDirection(bool state);

    QSize sizeHint() const override;

    /**
     * @brief Edge the button is located on.
     */
    Position position();

    /**
     * @brief Sets the container and child pane the button points to.
     */
    void setPane(DockingPaneContainer* container, DockingPaneBase* pane);

    /**
     * @brief The child pane the button points to.
     */
    DockingPaneBase* pane();

    /**
     * @brief The container that owns this auto-hide button.
     */
    DockingPaneContainer* container();

  protected:
    void paintEvent(QPaintEvent* event) override;
    void changeEvent(QEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

  Q_SIGNALS:
    void openFlyout();

  private:
    void init();
    /*
     * @brief Returns the StyleOption for the current object.
     */
    QStyleOptionButton* getStyleOption() const;
    /*
     * @brief Called when the hover timer elapses.
     */
    void onTimerElapsed();

  private:
    Qt::Orientation       m_orientation;
    bool                  m_mirrored;
    bool                  m_hovered;
    DockingPaneBase*      m_dockingPane;   // Pointed-to child pane (raw pointer).
    DockingPaneContainer* m_paneContainer; //  Pointed-to container (raw pointer).

    bool     m_swapDirection;
    Position m_pos;
    QTimer*  m_hoverTimer;
};

#endif // DOCKAUTOHIDEBUTTON_H
