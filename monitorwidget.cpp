#include "monitorwidget.h"
#include "network_monitor.h"
#include "netspeedplugin.h"
#include <QApplication>
#include <QVBoxLayout>

MonitorWidget::MonitorWidget(QWidget *parent) : QWidget(parent)

{
    updateSeconds = 2;
    setFixedWidth(319);
    setFixedHeight(98);
    QVBoxLayout *vbox = new QVBoxLayout;
    QHBoxLayout *hbox = new QHBoxLayout;
    networkMonitor = new NetworkMonitor;
    hbox->addWidget(networkMonitor);
    vbox->addLayout(hbox);
    vbox->addStretch();
    setLayout(vbox);
    connect(this, &MonitorWidget::updateNetworkStatus, this, &MonitorWidget::handleNetworkStatus, Qt::QueuedConnection);
    // Start timer.
    updateStatusTimer = new QTimer(this);
    connect(updateStatusTimer, SIGNAL(timeout()), this, SLOT(updateStatus()));
    updateStatusTimer->start(2000);
}

MonitorWidget::~MonitorWidget()
{
}

void MonitorWidget::getNetworkBandWidth(unsigned long long int &receiveBytes, unsigned long long int &sendBytes)
{
    char *buf;
    static int bufsize;
    FILE *devfd;

    buf = (char *) calloc(255, 1);
    bufsize = 255;
    devfd = fopen("/proc/net/dev", "r");

    // Ignore the first two lines of the file.
    fgets(buf, bufsize, devfd);
    fgets(buf, bufsize, devfd);

    receiveBytes = 0;
    sendBytes = 0;

    while (fgets(buf, bufsize, devfd)) {
        unsigned long long int rBytes, sBytes;
        char *line = strdup(buf);

        char *dev;
        dev = strtok(line, ":");

        // Filter lo (virtual network device).
        if (QString::fromStdString(dev).trimmed() != "lo") {
            sscanf(buf + strlen(dev) + 2, "%llu %*d %*d %*d %*d %*d %*d %*d %llu", &rBytes, &sBytes);

            receiveBytes += rBytes;
            sendBytes += sBytes;
        }

        free(line);
    }

    fclose(devfd);
    free(buf);
}

void MonitorWidget::updateStatus()
{
    // Update network status.
    if (prevTotalRecvBytes == 0) {
        prevTotalRecvBytes = totalRecvBytes;
        prevTotalSentBytes = totalSentBytes;

        getNetworkBandWidth(totalRecvBytes, totalSentBytes);
        updateNetworkStatus(totalRecvBytes, totalSentBytes, 0, 0);
    } else {
        prevTotalRecvBytes = totalRecvBytes;
        prevTotalSentBytes = totalSentBytes;

        getNetworkBandWidth(totalRecvBytes, totalSentBytes);
        updateNetworkStatus(totalRecvBytes,
                            totalSentBytes,
                            ((totalRecvBytes - prevTotalRecvBytes) / 1024.0) / updateSeconds,
                            ((totalSentBytes - prevTotalSentBytes) / 1024.0) / updateSeconds);
    }
}

void MonitorWidget::handleNetworkStatus(long totalRecvBytes, long totalSentBytes, float totalRecvKbs, float totalSentKbs)
{
        networkMonitor->update_Status(totalRecvBytes, totalSentBytes, totalRecvKbs, totalSentKbs);
}
