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

#include "network_monitor.h"
#include "smooth_curve_generator.h"
#include <QDebug>
#include <QPainter>
#include <QApplication>
#include <DHiDPIHelper>



NetworkMonitor::NetworkMonitor(QWidget *parent) : QWidget(parent)
{
    int MaxWidth = 321;
    setFixedWidth(MaxWidth);
    setFixedHeight(144);

    pointsNumber = int(MaxWidth / 5.4);

    downloadSpeeds = new QList<double>();
    for (int i = 0; i < pointsNumber; i++) {
        downloadSpeeds->append(0);
    }

    uploadSpeeds = new QList<double>();
    for (int i = 0; i < pointsNumber; i++) {
        uploadSpeeds->append(0);
    }
}

NetworkMonitor::~NetworkMonitor()
{
    delete downloadSpeeds;
    delete uploadSpeeds;
}

void NetworkMonitor::update_Status(long tRecvBytes, long tSentBytes, float tRecvKbs, float tSentKbs)
{
    totalRecvBytes = tRecvBytes;
    totalSentBytes = tSentBytes;
    totalRecvKbs = tRecvKbs;
    totalSentKbs = tSentKbs;

    // Init download path.
    downloadSpeeds->append(totalRecvKbs);

    if (downloadSpeeds->size() > pointsNumber) {
        downloadSpeeds->pop_front();
    }

    QList<QPointF> downloadPoints;

    double downloadMaxHeight = 0;
    for (int i = 0; i < downloadSpeeds->size(); i++) {
        if (downloadSpeeds->at(i) > downloadMaxHeight) {
            downloadMaxHeight = downloadSpeeds->at(i);
        }
    }

    for (int i = 0; i < downloadSpeeds->size(); i++) {
        if (downloadMaxHeight < downloadRenderMaxHeight) {
            downloadPoints.append(QPointF(i * 5, downloadSpeeds->at(i)));
        } else {
            downloadPoints.append(QPointF(i * 5, downloadSpeeds->at(i) * downloadRenderMaxHeight / downloadMaxHeight));
        }
    }

    downloadPath = SmoothCurveGenerator::generateSmoothCurve(downloadPoints);

    // Init upload path.
    uploadSpeeds->append(totalSentKbs);

    if (uploadSpeeds->size() > pointsNumber) {
        uploadSpeeds->pop_front();
    }

    QList<QPointF> uploadPoints;

    double uploadMaxHeight = 0;
    for (int i = 0; i < uploadSpeeds->size(); i++) {
        if (uploadSpeeds->at(i) > uploadMaxHeight) {
            uploadMaxHeight = uploadSpeeds->at(i);
        }
    }

    for (int i = 0; i < uploadSpeeds->size(); i++) {
        if (uploadMaxHeight < uploadRenderMaxHeight) {
            uploadPoints.append(QPointF(i * 5, uploadSpeeds->at(i)));
        } else {
            uploadPoints.append(QPointF(i * 5, uploadSpeeds->at(i) * uploadRenderMaxHeight / uploadMaxHeight));
        }
    }

    uploadPath = SmoothCurveGenerator::generateSmoothCurve(uploadPoints);

    repaint();
}

QString formatUnitSize(unsigned long v, QStringList orders)
{
   QString size;
    int order = 0;
    long double value = v;
    while (value >= 1024 && order + 1 < orders.size()) {
        order++;
        value  = value / 1024.0;
    }
    if (orders[order] == "KB/s")
    size = QString::number(static_cast<double>(value), 'r', 0);
    else
    size = QString::number(static_cast<double>(value), 'r', 1);
    return QString("%1 %2").arg(size).arg(orders[order]);
}

QString formatBandwidth(unsigned long v)
{
    QStringList orders;
    orders << "KB/s" << "MB/s" << "GB/s" << "TB/s";
    return formatUnitSize(v, orders);
}

QString formatByteCount(unsigned long v)
{
    QStringList orders;
    orders << "B" << "KB" << "MB" << "GB" << "TB";
    return formatUnitSize(v, orders);
}

void NetworkMonitor::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Draw background grid.
    painter.setRenderHint(QPainter::Antialiasing, false);
    QPen framePen;
    painter.setOpacity(0.15);
    framePen.setColor(QColor(textColor));
    framePen.setWidth(0.5);
    painter.setPen(framePen);

    int penSize = 1;
    int gridX = rect().x() + penSize;
    int gridY = rect().y() + gridRenderOffsetY + gridPaddingTop;
    int gridWidth = rect().width() - gridPaddingRight - penSize * 2;
    int gridHeight = downloadRenderMaxHeight + uploadRenderMaxHeight + waveformRenderPadding;

    QPainterPath framePath;
    framePath.addRect(QRect(gridX, gridY, gridWidth, gridHeight));
    painter.drawPath(framePath);

    // Draw grid.
    QPen gridPen;
    QVector<qreal> dashes;
    qreal space = 3;
    dashes << 5 << space;
    painter.setOpacity(0.1);
    gridPen.setColor(QColor(textColor));
    gridPen.setWidth(0.5);
    gridPen.setDashPattern(dashes);
    painter.setPen(gridPen);

    int gridLineX = gridX;
    while (gridLineX < gridX + gridWidth - gridSize) {
        gridLineX += gridSize;
        painter.drawLine(gridLineX, gridY + 1, gridLineX, gridY + gridHeight - 1);
    }
    int gridLineY = gridY;
    while (gridLineY < gridY + gridHeight - gridSize) {
        gridLineY += gridSize;
        painter.drawLine(gridX + 1, gridLineY, gridX + gridWidth - 1, gridLineY);
    }
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Draw network summary.
    QFont font = qApp->font();
    font.setPixelSize(14);
    font.setWeight(QFont::Bold);
    painter.setFont(font);
    QFontMetrics fm = painter.fontMetrics();

    QString downloadTitle = QString("%1").arg(formatBandwidth(totalRecvKbs));
    QString downloadContent = QString("%1").arg(formatByteCount(totalRecvBytes));
    QString uploadTitle = QString("%1").arg(formatBandwidth(totalSentKbs));
    QString uploadContent = QString("%1").arg(formatByteCount(totalSentBytes));
    QString downloadArrow = QString("🡇");
    QString uploadArrow = QString("🡅");
    int titleWidth = std::max(fm.width(downloadTitle), fm.width(uploadTitle));
    painter.setOpacity(0.7);
    painter.save();

    painter.translate((rect().width() - pointsNumber * 5) / 2 - 7, downloadWaveformsRenderOffsetY + gridPaddingTop);
    painter.scale(1, -1);

    qreal devicePixelRatio = qApp->devicePixelRatio();
    qreal networkCurveWidth = 2.5;
    if (devicePixelRatio > 1) {
        networkCurveWidth = 2.8;
    }
    painter.setPen(QPen(QColor(downloadColor), networkCurveWidth));
    painter.setBrush(QBrush());
    painter.drawPath(downloadPath);

    painter.translate(0, uploadWaveformsRenderOffsetY);
    painter.scale(1, -1);

    painter.setPen(QPen(QColor(uploadColor), networkCurveWidth));
    painter.setBrush(QBrush());
    painter.drawPath(uploadPath);

    painter.restore();
    painter.setPen(QPen(QColor(downloadColor)));
    painter.setBrush(QBrush(QColor(downloadColor)));
    painter.drawText(QRect(rect().x() + arrowRenderPaddingX,
                           rect().y() + downloadRenderPaddingY,
                           fm.width(downloadArrow),
                           rect().height()),
                     Qt::AlignLeft | Qt::AlignTop,
                     downloadArrow);

    painter.setPen(QPen(QColor(summaryColor)));
    painter.drawText(QRect(rect().x() + downloadRenderPaddingX,
                           rect().y() + downloadRenderPaddingY,
                           fm.width(downloadTitle),
                           rect().height()),
                     Qt::AlignLeft | Qt::AlignTop,
                     downloadTitle);

    painter.setPen(QPen(QColor(summaryColor)));
    painter.drawText(QRect(rect().x() + downloadRenderPaddingX + titleWidth + textPadding,
                           rect().y() + downloadRenderPaddingY,
                           fm.width(downloadContent),
                           rect().height()),
                     Qt::AlignLeft | Qt::AlignTop,
                     downloadContent);

    painter.setPen(QPen(QColor(uploadColor)));
    painter.setBrush(QBrush(QColor(uploadColor)));
    painter.drawText(QRect(rect().x() + arrowRenderPaddingX,
                           rect().y() + uploadRenderPaddingY,
                           fm.width(uploadArrow), rect().height()),
                     Qt::AlignLeft | Qt::AlignTop,
                     uploadArrow);

    painter.setPen(QPen(QColor(summaryColor)));
    painter.drawText(QRect(rect().x() + uploadRenderPaddingX,
                           rect().y() + uploadRenderPaddingY,
                           fm.width(uploadTitle),
                           rect().height()),
                     Qt::AlignLeft | Qt::AlignTop,
                     uploadTitle);

    painter.setPen(QPen(QColor(summaryColor)));
    painter.drawText(QRect(rect().x() + uploadRenderPaddingX + titleWidth + textPadding,
                           rect().y() + uploadRenderPaddingY,
                           fm.width(uploadContent),
                           rect().height()),
                     Qt::AlignLeft | Qt::AlignTop,
                     uploadContent);
}
