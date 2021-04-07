#ifndef MONITORWIDGET_H
#define MONITORWIDGET_H

#include "network_monitor.h"
#include <QTimer>
#include <proc/readproc.h>
#include <dimagebutton.h>

DWIDGET_USE_NAMESPACE

class MonitorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MonitorWidget(QWidget *parent = nullptr);
    ~MonitorWidget();

    DImageButton *floatButton;
    void setColor(const QString &color);

signals:
    void updateNetworkStatus(long totalRecvBytes, long totalSentBytes, float totalRecvKbs, float totalSentKbs);

public slots:
    void updateStatus();
    void getNetworkBandWidth(unsigned long long int &receiveBytes, unsigned long long int &sendBytes);

private:
    qreal updateSeconds = 2;
    unsigned long long int prevTotalRecvBytes;
    unsigned long long int prevTotalSentBytes;
    unsigned long long int nextTotalRecvBytes;
    unsigned long long int nextTotalSentBytes;
    NetworkMonitor *m_networkMonitor;
    QTimer *updateStatusTimer;
};

#endif // MONITORWIDGET_H