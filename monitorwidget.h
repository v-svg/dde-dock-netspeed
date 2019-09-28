#ifndef MONITORWIDGET_H
#define MONITORWIDGET_H

#include "network_monitor.h"
#include <QWidget>
#include <QTimer>
#include <proc/readproc.h>

class MonitorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MonitorWidget(QWidget *parent = nullptr);
    ~MonitorWidget();

signals:
    void updateNetworkStatus(long totalRecvBytes, long totalSentBytes, float totalRecvKbs, float totalSentKbs);

public slots:
    void updateStatus();
    void getNetworkBandWidth(unsigned long long int &receiveBytes, unsigned long long int &sendBytes);
    void handleNetworkStatus(long totalRecvBytes, long totalSentBytes, float totalRecvKbs, float totalSentKbs);

private:
    qreal updateSeconds;
    unsigned long long int prevTotalRecvBytes;
    unsigned long long int prevTotalSentBytes;
    unsigned long long int totalRecvBytes;
    unsigned long long int totalSentBytes;
    NetworkMonitor *networkMonitor;
    QTimer *updateStatusTimer;
};

#endif // MONITORWIDGET_H