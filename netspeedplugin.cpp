#include "netspeedplugin.h"

#include <DApplication>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QDialog>
#include <QTimer>

DWIDGET_USE_NAMESPACE

NetspeedPlugin::NetspeedPlugin(QObject *parent)
    : QObject(parent)
    , m_tipsLabel(new QLabel)
    , m_refershTimer(new QTimer(this))
    , m_settings("deepin", "dde-dock-netspeed")
    , m_netspeedItem(new NetspeedItem)
    , m_monitorWidget(new MonitorWidget)
    , m_floatingWidget(new FloatingWidget)
{
    QString applicationName = qApp->applicationName();
    qApp->setApplicationName("dde-dock-netspeed");
    qDebug() << qApp->loadTranslator();
    qApp->setApplicationName(applicationName);

    i = ds = us = db = ub = dbt1 = ubt1 = dbt0 = ubt0 = 0;
    m_tipsLabel->setObjectName("netspeed");
    m_tipsLabel->setStyleSheet("padding: 0px 2px;");
    m_refershTimer->setInterval(1000);
    m_refershTimer->start();


    connect(m_monitorWidget->floatButton, &DImageButton::clicked, this, &NetspeedPlugin::showFloatingWidget);
    connect(m_netspeedItem, &NetspeedItem::mouseMidBtnClicked, this, &NetspeedPlugin::showFloatingWidget);
    connect(m_netspeedItem, &NetspeedItem::requestUpdateGeometry, [this] {
            m_proxyInter->itemUpdate(this, pluginName());
    });
    connect(m_refershTimer, &QTimer::timeout, this, &NetspeedPlugin::updateNetspeed);
    connect(m_floatingWidget, &FloatingWidget::isHidden,  [this] {
            m_monitorWidget->floatButton->show();
    });
    // Boot time
    QProcess *process = new QProcess;
    process->start("systemd-analyze");
    process->waitForFinished();
    QString PO = process->readAllStandardOutput();
    QString SD = PO.mid(PO.indexOf("=") + 1, PO.indexOf("\n") - PO.indexOf("=") - 1);
    SD.replace("s", " s");
    startup = "🚀 " + SD;
}

NetspeedPlugin::~NetspeedPlugin()
{
}

const QString NetspeedPlugin::pluginName() const
{
    return "netspeed";
}

const QString NetspeedPlugin::pluginDisplayName() const
{
    return tr("Network speed");
}

void NetspeedPlugin::init(PluginProxyInterface *proxyInter)
{
    m_proxyInter = proxyInter;
    if (m_netspeedItem->enabled())
        m_proxyInter->itemAdded(this, pluginName());
}

void NetspeedPlugin::pluginStateSwitched()
{
    m_netspeedItem->setEnabled(!m_netspeedItem->enabled());
    if (m_netspeedItem->enabled())
        m_proxyInter->itemAdded(this, pluginName());
    else
        m_proxyInter->itemRemoved(this, pluginName());
}

bool NetspeedPlugin::pluginIsDisable()
{
    return !m_netspeedItem->enabled();
}

int NetspeedPlugin::itemSortKey(const QString &itemKey)
{
    Q_UNUSED(itemKey);

    const QString key = QString("pos_%1").arg(displayMode());
    return m_settings.value(key, 0).toInt();
}

void NetspeedPlugin::setSortKey(const QString &itemKey, const int order)
{
    Q_UNUSED(itemKey);

    const QString key = QString("pos_%1").arg(displayMode());
    m_settings.setValue(key, order);
}

QWidget *NetspeedPlugin::itemWidget(const QString &itemKey)
{
    Q_UNUSED(itemKey);
    return m_netspeedItem;
}

QWidget *NetspeedPlugin::itemTipsWidget(const QString &itemKey)
{
    Q_UNUSED(itemKey);
    return m_tipsLabel;
}

QWidget *NetspeedPlugin::itemPopupApplet(const QString &itemKey)
{
    Q_UNUSED(itemKey);

    QColor textColor = QApplication::palette().text().color();
    m_monitorWidget->setColor(textColor.name());

    if (textColor == Qt::white) {
        m_monitorWidget->floatButton->setNormalPic(":/images/light_up_normal.svg");
        m_monitorWidget->floatButton->setHoverPic(":/images/light_up_hover.svg");
        m_monitorWidget->floatButton->setPressPic(":/images/light_up_press.svg");
    } else {
        m_monitorWidget->floatButton->setNormalPic(":/images/dark_up_normal.svg");
        m_monitorWidget->floatButton->setHoverPic(":/images/dark_up_hover.svg");
        m_monitorWidget->floatButton->setPressPic(":/images/dark_up_press.svg");
    }

    if (!m_floatingWidget->isVisible()) {
        return m_monitorWidget;
    } else {
        return nullptr;
    }
}

const QString NetspeedPlugin::itemContextMenu(const QString &itemKey)
{
    Q_UNUSED(itemKey);

    QList<QVariant> items;
    items.reserve(2);

    QMap<QString, QVariant> boot_analyze;
    boot_analyze["itemId"] = "boot_analyze";
    boot_analyze["itemText"] = tr("Boot analyze");
    boot_analyze["isActive"] = true;
    items.push_back(boot_analyze);

    QMap<QString, QVariant> boot_record;
    boot_record["itemId"] = "boot_record";
    boot_record["itemText"] = tr("Boot record");
    boot_record["isActive"] = true;
    items.push_back(boot_record);

    QMap<QString, QVariant> system_monitor;
    system_monitor["itemId"] = "system_monitor";
    system_monitor["itemText"] = tr("System monitor");
    system_monitor["isActive"] = true;
    items.push_back(system_monitor);

    QMap<QString, QVariant> menu;
    menu["items"] = items;
    menu["checkableMenu"] = false;
    menu["singleCheck"] = false;
    return QJsonDocument::fromVariant(menu).toJson();
}

void NetspeedPlugin::invokedMenuItem(const QString &itemKey, const QString &menuId, const bool checked)
{
    Q_UNUSED(itemKey);
    Q_UNUSED(checked);
    if(menuId == "boot_analyze")
        bootAnalyze();
    if(menuId == "boot_record")
        bootRecord();
    if(menuId == "system_monitor")
        QProcess::startDetached("deepin-system-monitor");
}

QString NetspeedPlugin::KB(long k)
{
    QString s = "";
    if (k > 999999)
        s = QString::number(k / (1024 * 1024.0), 'f', 1) + " GB";
    else {
        if (k > 999)
            s = QString::number(k / 1024.0, 'f', 0) + " MB";
        else
            s = QString::number(k / 1.0, 'f', 0) + " KB";
    }
    return s;
}

QString NetspeedPlugin::BS(long b)
{
    QString s = "";
    if (b > 9999999999)
        s = QString::number(b / (1024 * 1024 * 1024.0), 'f', 1) + " GB";
    else {
        if (b > 999999999)
            s = QString::number(b / (1024 * 1024 * 1024.0), 'f', 2) + " GB";
        else {
            if (b > 999999)
                s = QString::number(b / (1024 * 1024.0), 'f', 1) + " MB";
            else {
                if (b > 999)
                    s = QString::number(b / (1024.0), 'f', 0) + " KB";
                else
                    s = b + " B";
            }
        }
    }
    return s;
}

QString NetspeedPlugin::NB(long b)
{
    QString s = "";
    if (b > 999999999)
        s = QString::number(b / (1024 * 1024 * 1024.0), 'f', 2) + " GB";
    else {
        if (b > 99999999)
            s = QString::number(b / (1024 * 1024.0), 'f', 0) + " MB";
        else {
            if (b > 9999999)
                s = QString::number(b / (1024 * 1024.0), 'f', 1) + " MB";
            else {
                if (b > 999999)
                    s = QString::number(b / (1024 * 1024.0), 'f', 2) + " MB";
	           else {		
		          if (b > 999)
                        s = QString::number(b / 1024, 'f', 0) + " KB";
		          else
                        s = "0 KB";
                }
            }
        }
    }
    return s;
}

void NetspeedPlugin::updateNetspeed()
{
    // uptime
    QFile file("/proc/uptime");
    file.open(QIODevice::ReadOnly);
    QString line = file.readLine();
    file.close();
    QTime time(0, 0, 0);
    time = time.addSecs(line.left(line.indexOf(".")).toInt());
    QString uptime = "⏰  " + time.toString("hh:mm:ss");

    // CPU
    file.setFileName("/sys/class/thermal/thermal_zone0/temp");
    if (file.open(QIODevice::ReadOnly)) {
        line = file.readLine();
        file.close();
    } else {
        file.setFileName("/sys/class/hwmon/hwmon0/temp1_input");
        file.open(QIODevice::ReadOnly);
        line = file.readLine();
        file.close();
    }
    double lt = line.toDouble() / 1000;

    file.setFileName("/sys/devices/system/cpu/cpufreq/policy0/scaling_cur_freq");
    double ln;
    if (file.open(QIODevice::ReadOnly)) {
        line = file.readLine();
        file.close();
        ln = line.toDouble() / 1000000;
    } else {
        file.setFileName("/proc/cpuinfo");
        file.open(QIODevice::ReadOnly);
        for (int st = 0; st < 8; st++)
            line = file.readLine();
        int pos = line.indexOf(": ");
        pos += 2;
        line = line.mid(pos);
        file.close();
        ln = line.toDouble() / 1000;
    }

    file.setFileName("/proc/stat");
    file.open(QIODevice::ReadOnly);
    line = file.readLine();
    QByteArray ba;
    ba = line.toLatin1();
    const char *ch;
    ch = ba.constData();
    char cpu[5];
    long user, nice, sys, idle, iowait, irq, softirq, tt;
    sscanf(ch, "%s%ld%ld%ld%ld%ld%ld%ld", cpu, &user, &nice, &sys, &idle, &iowait, &irq, &softirq);
    tt = user + nice + sys + idle + iowait + irq + softirq;
    file.close();
    QString cusage = "";
    int cp = ((tt - tt0) - (idle - idle0)) * 100 / (tt - tt0);
    if (i > 0)
        cusage = "🏾  " + QString::number(ln, 'f', 2) + " GHz  " + QString::number(lt) + " °C  " + QString::number(cp) + "%";
    idle0 = idle;
    tt0 = tt;

    // memory
    file.setFileName("/proc/meminfo");
    file.open(QIODevice::ReadOnly);
    line = file.readLine();
    long mt = line.replace("MemTotal:", "").replace("kB", "").replace(" ", "").toLong();
    line = file.readLine();
    line = file.readLine();
    long ma = line.replace("MemAvailable:", "").replace("kB", "").replace(" ", "").toLong();
    file.close();
    long mu = mt - ma;
    int mp = mu * 100 / mt;
    QString mem = "🝚  " + QString("%1 / %2 %3").arg(KB(mu)).arg(KB(mt)).arg(" " + QString::number(mp) + "%");

    // net
    file.setFileName("/proc/net/dev");
    file.open(QIODevice::ReadOnly);
    line = file.readLine();
    line = file.readLine();
    dbt1 = ubt1 = 0;
    while (!file.atEnd()) {
        line = file.readLine();
        QStringList list = line.split(QRegExp("\\s{1,}"));
        db = list.at(1).toLong();
        ub = list.at(9).toLong();
        dbt1 += db;
        ubt1 += ub;
    }
    file.close();
    QString dss = "0 KB/s";
    QString uss = "0 KB/s";
    QString oss = "0 KB/s";
    if (i > 0) {
        ds = dbt1 - dbt0;
        us = ubt1 - ubt0;
        dss = NB(ds) + "/s";
        uss = NB(us) + "/s";
        if (ds > us)
            oss = NB(ds) + "/s";
        else
            oss = NB(us) + "/s";
        dbt0 = dbt1;
        ubt0 = ubt1;
    }
    QString net = "🡇  " + dss + "  " + BS(dbt1) + "\n🡅  " + uss + "  " + BS(ubt1);
    i++;
    if (i > 2)
        i = 2;

    // draw
    m_tipsLabel->setText(startup + "\n" + uptime + "\n" + cusage + "\n" + mem + "\n" + net);

    m_netspeedItem->text = oss;

    if (ds > 999)
        m_netspeedItem->download = 1;
    else
        m_netspeedItem->download = 0;
    if (us > 999)
        m_netspeedItem->upload = 1;
    else
        m_netspeedItem->upload = 0;

    m_netspeedItem->update();

}

void NetspeedPlugin::bootRecord()
{
    QProcess *process = new QProcess;
    process->start("/bin/bash -c \"last -x -R echo \"$USER\"\"");
    process->waitForFinished();

    QTextEdit *textEdit = new QTextEdit;
    textEdit->setText(process->readAllStandardOutput());
    textEdit->setReadOnly(true);

    QPushButton *button = new QPushButton("OK");

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 11);
    layout->setSpacing(9);
    layout->addWidget(textEdit);
    layout->addWidget(button, 0, Qt::AlignCenter);

    QDialog *dialog = new QDialog;
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle(tr("Boot record"));
    dialog->setWindowIcon(QIcon::fromTheme("gshutdown"));
    dialog->setFixedSize(500, 400);
    dialog->setLayout(layout);
    dialog->open();

    connect(button, &QPushButton::clicked, dialog, &QDialog::close);
}

void NetspeedPlugin::bootAnalyze()
{
    QProcess *process = new QProcess;
    process->start("systemd-analyze blame");
    process->waitForFinished();

    QTextEdit *textEdit = new QTextEdit;
    textEdit->setText(process->readAllStandardOutput());
    textEdit->setReadOnly(true);

    QPushButton *button = new QPushButton("OK");

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 11);
    layout->setSpacing(9);
    layout->addWidget(textEdit);
    layout->addWidget(button, 0, Qt::AlignCenter);

    QDialog *dialog = new QDialog;
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle(tr("Boot analyze"));
    dialog->setWindowIcon(QIcon::fromTheme("kronometer"));
    dialog->setFixedSize(500, 400);
    dialog->setLayout(layout);
    dialog->open();

    connect(button, &QPushButton::clicked, dialog, &QDialog::close);
}

void NetspeedPlugin::showFloatingWidget()
{
    if (!m_floatingWidget->isVisible()) {
        if (m_monitorWidget->isVisible())
            m_monitorWidget->parentWidget()->hide();
        m_monitorWidget->floatButton->hide();
        m_floatingWidget->show();
    } else {
        m_monitorWidget->floatButton->show();
        m_floatingWidget->hide();
    }
}
