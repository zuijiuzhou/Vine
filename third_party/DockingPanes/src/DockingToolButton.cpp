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

#include <QDebug>
#include <QImage>
#include <QPaintEvent>
#include <QPainter>

#include "DockingPaneTheme.h"
#include "DockingToolButton.h"

DockingToolButton::DockingToolButton(DockingToolButton::ButtonType type, QWidget* parent)
  : QPushButton(parent)
{
    setMouseTracking(true);

    m_buttonType = type;
    m_highlight  = false;
}

void DockingToolButton::enterEvent(QEnterEvent* event)
{
    m_highlight = true;

    QPushButton::enterEvent(event);
}

void DockingToolButton::leaveEvent(QEvent* event)
{
    m_highlight = false;

    QPushButton::leaveEvent(event);
}

void DockingToolButton::paintEvent(QPaintEvent*)
{
    QImage   buttonImage;
    QPainter p(this);

    int centreX = width() / 2;
    int centreY = height() / 2;

    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::Antialiasing, true);

    switch (m_buttonType) {
    case closeButtonInactive:
    {
        buttonImage.load(":/img/tool_button_bitmaps/close_inactive.png");
        break;
    }

    case closeButtonActive:
    {
        buttonImage.load(":/img/tool_button_bitmaps/close_active.png");
        break;
    }

    case pinButtonInactive:
    {
        buttonImage.load(":/img/tool_button_bitmaps/pin_inactive.png");
        break;
    }

    case pinButtonActive:
    {
        buttonImage.load(":/img/tool_button_bitmaps/pin_active.png");
        break;
    }

    case unpinButtonInactive:
    {
        buttonImage.load(":/img/tool_button_bitmaps/pin_inactive.png");
        buttonImage = buttonImage.transformed(QTransform().rotate(90));
        break;
    }

    case unpinButtonActive:
    {
        buttonImage.load(":/img/tool_button_bitmaps/pin_active.png");
        buttonImage = buttonImage.transformed(QTransform().rotate(90));
        break;
    }
    }

    // Both the white "active" bitmaps and the black "inactive" ones are
    // re-coloured with a palette-driven colour so the icons stay visible on
    // every theme: the white bitmaps would vanish on light headers, the
    // black ones on dark headers. The PNGs load as Format_Indexed8, which
    // cannot be painted on, so convert first.
    const bool activeButton = (m_buttonType == closeButtonActive || m_buttonType == pinButtonActive || m_buttonType == unpinButtonActive);

    if (!buttonImage.isNull()) {
        buttonImage = buttonImage.convertToFormat(QImage::Format_ARGB32);

        QPainter tintPainter(&buttonImage);

        tintPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
        tintPainter.fillRect(buttonImage.rect(), DockingPaneTheme::iconTintColor(activeButton));
    }

    p.drawImage(centreX - (buttonImage.width() / 2), centreY - (buttonImage.height() / 2), buttonImage);

    QRect rcHighlight;

    rcHighlight.setLeft(centreX - (buttonImage.width() / 2) - 2);
    rcHighlight.setRight(centreX + (buttonImage.width() / 2) + 2);

    rcHighlight.setTop(centreY - (buttonImage.height() / 2) - 2);
    rcHighlight.setBottom(centreY + (buttonImage.height() / 2) + 2);

    if (m_highlight) {
        p.fillRect(rcHighlight, QColor(0xFF, 0xFF, 0xFF, 0xAA));
    }
}

void DockingToolButton::setButton(DockingToolButton::ButtonType type)
{
    m_buttonType = type;
    update();
}

void DockingToolButton::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::PaletteChange || event->type() == QEvent::ApplicationPaletteChange) {
        update();
    }

    QPushButton::changeEvent(event);
}
