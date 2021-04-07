/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "floatingwidget.h"

#include <QHBoxLayout>
#include <QDesktopWidget>
#include <QApplication>

#include <DPlatformWindowHandle>

FloatingWidget::FloatingWidget(QWidget *parent)
    : DBlurEffectWidget(parent)
    , m_settings("deepin", "dde-dock-netspeed")
    , m_monitorWidget(new MonitorWidget)
{
    setWindowFlags(Qt::WindowDoesNotAcceptFocus | Qt::WindowStaysOnTopHint |
                   Qt::FramelessWindowHint | Qt::Tool);

    setFixedWidth(329);
    setFixedHeight(109);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->addWidget(m_monitorWidget);
    setLayout(layout);

    m_monitorWidget->floatButton->hide();

    DPlatformWindowHandle handle(this);
    handle.setShadowRadius(40);
    handle.setShadowOffset(QPoint(0, 5));

    m_alphaButton = new DImageButton(this);
    m_alphaButton->setFixedSize(16, 16);
    m_alphaButton->move(237, 24);
    m_alphaButton->hide();

    m_themeButton = new DImageButton(this);
    m_themeButton->setFixedSize(16, 16);
    m_themeButton->move(263, 24);
    m_themeButton->hide();

    m_closeButton = new DImageButton(this);
    m_closeButton->setFixedSize(16, 16);
    m_closeButton->move(289, 24);
    m_closeButton->hide();

    setMaskAlpha(m_settings.value("alpha", 102).toInt());

    setTheme();

    connect(m_alphaButton, &DImageButton::clicked, this, &FloatingWidget::setOpacity);
    connect(m_themeButton, &DImageButton::clicked, [this] {
        if (m_settings.value("light").toBool())
            m_settings.setValue("light", false);
        else                
            m_settings.setValue("light", true);
        setTheme();                
    });
    connect(m_closeButton, &DImageButton::clicked, [this] {
        hide();
        emit isHidden();
    });
}

void FloatingWidget::setColor(const QString &color)
{
    m_monitorWidget->setColor(color);
}

void FloatingWidget::setOpacity()
{
    quint8 value = maskAlpha();
    if (value < 255) {
        if (value == 63 || value == 216)
            value += 39;
        else
            value += 38;
    } else
        value = 63;

    setMaskAlpha(value);

    m_settings.setValue("alpha", value);
}

void FloatingWidget::setTheme()
{
    if (m_settings.value("light").toBool()) {
        m_monitorWidget->setColor("#191919");

        m_alphaButton->setNormalPic(":/images/dark_opacity_normal.svg");
        m_alphaButton->setHoverPic(":/images/dark_opacity_hover.svg");
        m_alphaButton->setPressPic(":/images/dark_opacity_press.svg");

        m_themeButton->setNormalPic(":/images/dark_theme_normal.svg");
        m_themeButton->setHoverPic(":/images/dark_theme_hover.svg");
        m_themeButton->setPressPic(":/images/dark_theme_press.svg");

        m_closeButton->setNormalPic(":/images/dark_close_normal.svg");
        m_closeButton->setHoverPic(":/images/dark_close_hover.svg");
        m_closeButton->setPressPic(":/images/dark_close_press.svg");

        setMaskColor(DBlurEffectWidget::LightColor);
    } else {
        m_monitorWidget->setColor("#FFFFFF");

        m_alphaButton->setNormalPic(":/images/light_opacity_normal.svg");
        m_alphaButton->setHoverPic(":/images/light_opacity_hover.svg");
        m_alphaButton->setPressPic(":/images/light_opacity_press.svg");

        m_themeButton->setNormalPic(":/images/light_theme_normal.svg");
        m_themeButton->setHoverPic(":/images/light_theme_hover.svg");
        m_themeButton->setPressPic(":/images/light_theme_press.svg");

        m_closeButton->setNormalPic(":/images/light_close_normal.svg");
        m_closeButton->setHoverPic(":/images/light_close_hover.svg");
        m_closeButton->setPressPic(":/images/light_close_press.svg");

        setMaskColor(DBlurEffectWidget::DarkColor);
    }
}

void FloatingWidget::showEvent(QShowEvent *e)
{
    move(m_settings.value("pos", QPoint(200, 200)).toPoint());

    setPositionVisible();

    DBlurEffectWidget::showEvent(e);
}

void FloatingWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
        m_settings.setValue("pos", pos());
}

void FloatingWidget::enterEvent(QEvent *e)
{
    if (!m_dblClicked) {
        m_alphaButton->show();
        m_themeButton->show();
        m_closeButton->show();
    }

    DBlurEffectWidget::enterEvent(e);
}

void FloatingWidget::leaveEvent(QEvent *e)
{
    if (!m_dblClicked) {
        m_alphaButton->hide();
        m_themeButton->hide();
        m_closeButton->hide();
    }

    DBlurEffectWidget::leaveEvent(e);
}

void FloatingWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e)

    if (m_dblClicked) {
        m_dblClicked = false;

        m_alphaButton->hide();
        m_themeButton->hide();
        m_closeButton->hide();
    } else {
        m_dblClicked = true;

        m_alphaButton->show();
        m_themeButton->show();
        m_closeButton->show();
    }
}

void FloatingWidget::setPositionVisible()
{
    QDesktopWidget *desktop = QApplication::desktop();
    const int primary = desktop->primaryScreen();
    const QRect primaryRect = desktop->screenGeometry(primary);

    int x = this->x();
    int y = this->y();

    if (x + frameGeometry().width() > primaryRect.right())
        x = primaryRect.right() - frameGeometry().width();
    if (x < primaryRect.x())
        x = primaryRect.x();

    if (y + frameGeometry().height() > primaryRect.bottom())
        y = primaryRect.bottom() - frameGeometry().height();
    if (y < primaryRect.y())
        y = primaryRect.y();

    move(x, y);
}
