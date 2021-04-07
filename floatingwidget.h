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

#ifndef FLOATINGWIDGET_H
#define FLOATINGWIDGET_H

#include "monitorwidget.h"

#include <QMouseEvent>
#include <QSettings>

#include <DBlurEffectWidget>

DWIDGET_USE_NAMESPACE

class FloatingWidget : public DBlurEffectWidget
{
    Q_OBJECT
public:
    explicit FloatingWidget(QWidget *parent = 0);

    void setColor(const QString &color);

signals:
    void isHidden();

protected:
    void showEvent(QShowEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);

private slots:
    void setOpacity();
    void setTheme();

private:
    QSettings m_settings;
    bool m_dblClicked = false;
    MonitorWidget *m_monitorWidget;
    DImageButton *m_alphaButton;
    DImageButton *m_themeButton;
    DImageButton *m_closeButton;
    const QRect primaryRect;
    void setPositionVisible();
};

#endif // FLOATINGWIDGET_H
