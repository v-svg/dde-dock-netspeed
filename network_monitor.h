/* -*- Mode: C++; indent-tabs-mode: nil; tab-width: 4 -*-
 * -*- coding: utf-8 -*-
 *
 * Copyright (C) 2011 ~ 2018 Deepin, Inc.
 *               2011 ~ 2018 Wang Yong
 *
 * Author:     Wang Yong <wangyong@deepin.com>
 * Maintainer: Wang Yong <wangyong@deepin.com>
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

#ifndef NETWORKMONITOR_H
#define NETWORKMONITOR_H

#include <QWidget>

class NetworkMonitor : public QWidget
{
    Q_OBJECT
    
public:
    NetworkMonitor(QWidget *parent = 0);
    ~NetworkMonitor();
    
public slots:
    void update_Status(long totalRecvBytes, long totalSentBytes, float totalRecvKbs, float totalSentKbs);
    
protected:
    void paintEvent(QPaintEvent *event);
    
private:
    QList<double> *downloadSpeeds;
    QList<double> *uploadSpeeds;
    QPainterPath downloadPath;
    QPainterPath uploadPath;
    QString downloadColor = "#55D500";
    QString summaryColor = "#FFFFFF";
    QString textColor = "#FFFFFF";
    QString uploadColor = "#FF9800";
    float totalRecvKbs = 0;
    float totalSentKbs = 0;
    int downloadRenderMaxHeight = 50;
    int downloadRenderPaddingX = 30;
    int downloadRenderPaddingY = 10;
    int downloadWaveformsRenderOffsetX = 4;
    int downloadWaveformsRenderOffsetY = 60;
    int gridPaddingRight = 21;
    int gridPaddingTop = 0;
    int gridRenderOffsetY = 0;
    int gridSize = 20;
    int arrowRenderPaddingX = 10;
    int pointsNumber = 51;
    int textPadding = 8;
    int uploadRenderMaxHeight = 10;
    int uploadRenderPaddingX = 30;
    int uploadRenderPaddingY = 30;
    int uploadWaveformsRenderOffsetY = -4;
    int waveformRenderPadding = 20;
    long totalRecvBytes = 0;
    long totalSentBytes = 0;
};

#endif    
