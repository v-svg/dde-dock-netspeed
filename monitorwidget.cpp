#include "monitorwidget.h"

#include <QVBoxLayout>

MonitorWidget::MonitorWidget(QWidget *parent) : QWidget(parent)

{
    setFixedWidth(319);
    setFixedHeight(99);

    m_networkMonitor = new NetworkMonitor;
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->setContentsMargins(8, 8, 8, 8);
    vbox->addWidget(m_networkMonitor);
    setLayout(vbox);

    floatButton = new DImageButton(this);
    floatButton->setFixedSize(16, 16);
    floatButton->move(284, 19);

    updateStatusTimer = new QTimer(this);
    updateStatusTimer->start(2000);

    connect(updateStatusTimer, &QTimer::timeout, this, &MonitorWidget::updateStatus);
    connect(this, &MonitorWidget::updateNetworkStatus,
            m_networkMonitor, &NetworkMonitor::update_Status, Qt::QueuedConnection);
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
        prevTotalRecvBytes = nextTotalRecvBytes;
        prevTotalSentBytes = nextTotalSentBytes;

        getNetworkBandWidth(nextTotalRecvBytes, nextTotalSentBytes);
        updateNetworkStatus(nextTotalRecvBytes, nextTotalSentBytes, 0, 0);
    } else {
        prevTotalRecvBytes = nextTotalRecvBytes;
        prevTotalSentBytes = nextTotalSentBytes;

        getNetworkBandWidth(nextTotalRecvBytes, nextTotalSentBytes);
        updateNetworkStatus(nextTotalRecvBytes, nextTotalSentBytes,
                            ((nextTotalRecvBytes - prevTotalRecvBytes) / 1024.0) / updateSeconds,
                            ((nextTotalSentBytes - prevTotalSentBytes) / 1024.0) / updateSeconds);
    }
}

void MonitorWidget::setColor(const QString &color)
{
    m_networkMonitor->setColor(color);
}
