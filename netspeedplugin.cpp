#include "netspeedplugin.h"
#include "monitorwidget.h"
#include <QLabel>
#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextBrowser>
#include <QPushButton>

NetspeedPlugin::NetspeedPlugin(QObject *parent)
    : QObject(parent),
      m_tipsLabel(new QLabel),
      m_refershTimer(new QTimer(this)),
      m_settings("deepin", "dde-dock-netspeed")
{
    i = ds = us = db = ub = dbt1 = ubt1 = dbt0 = ubt0 = 0;
    m_tipsLabel->setObjectName("netspeed");
    m_tipsLabel->setStyleSheet("color:white; padding:0px 2px;");
    m_refershTimer->setInterval(1000);
    m_refershTimer->start();
    m_centralWidget = new NetspeedWidget;
    m_netspeedWidget = new MonitorWidget;
    connect(m_centralWidget, &NetspeedWidget::requestUpdateGeometry, [this] { m_proxyInter->itemUpdate(this, pluginName()); });
    connect(m_refershTimer, &QTimer::timeout, this, &NetspeedPlugin::updateNetspeed);

    // Boot time
    QProcess *process = new QProcess;
    process->start("systemd-analyze");
    process->waitForFinished();
    QString PO = process->readAllStandardOutput();
    QString SD = PO.mid(PO.indexOf("=") + 1, PO.indexOf("\n") - PO.indexOf("=") - 1);
    SD.replace(QString("s"), QString(" s"));
    startup = "⏱ " + SD;

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
    if (m_centralWidget->enabled())
        m_proxyInter->itemAdded(this, pluginName());
}

void NetspeedPlugin::pluginStateSwitched()
{
    m_centralWidget->setEnabled(!m_centralWidget->enabled());
    if (m_centralWidget->enabled())
        m_proxyInter->itemAdded(this, pluginName());
    else
        m_proxyInter->itemRemoved(this, pluginName());
}

bool NetspeedPlugin::pluginIsDisable()
{
    return !m_centralWidget->enabled();
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
    return m_centralWidget;
}

QWidget *NetspeedPlugin::itemTipsWidget(const QString &itemKey)
{
    Q_UNUSED(itemKey);
    return m_tipsLabel;
}

QWidget *NetspeedPlugin::itemPopupApplet(const QString &itemKey)
{
    Q_UNUSED(itemKey);
    m_netspeedWidget->update();
    return m_netspeedWidget;
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
        if (k > 999) s = QString::number(k / 1024.0, 'f', 0) + " MB";
        else s = QString::number(k / 1.0, 'f', 0) + " KB";
    }
    return s;
}

QString NetspeedPlugin::BS(long b)
{
    QString s = "";
    if (b > 9999999999) s = QString::number(b / (1024 * 1024 * 1024.0), 'f', 1) + " GB";
    else {
        if (b > 999999999) s = QString::number(b / (1024 * 1024 * 1024.0), 'f', 2) + " GB";
        else {
            if (b > 999999) s = QString::number(b / (1024 * 1024.0), 'f', 1) + " MB";
            else {
                if (b > 999) s = QString::number(b / (1024.0), 'f', 0) + " KB";
                else s = b + " B";
            }
        }
    }
    return s;
}

QString NetspeedPlugin::NB(long b)
{
    QString s = "";
    if (b > 999999999) s = QString::number(b / (1024 * 1024 * 1024.0), 'f', 2) + " GB";
    else {
        if (b > 99999999) s = QString::number(b / (1024 * 1024.0), 'f', 0) + " MB";
        else {
            if (b > 9999999) s = QString::number(b / (1024 * 1024.0), 'f', 1) + " MB";
            else {
                if (b > 999999) s = QString::number(b / (1024 * 1024.0), 'f', 2) + " MB";
	           else {		
		          if (b > 999) s = QString::number(b / 1024, 'f', 0) + " KB";
		          else s = QString::number(0, 'f', 0) + " KB";
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
    QString l = file.readLine();
    file.close();
    QTime t(0,0,0);
    t = t.addSecs(l.left(l.indexOf(".")).toInt());
    QString uptime = "⏲  " + t.toString("hh:mm:ss");

    // CPU
    file.setFileName("/sys/class/hwmon/hwmon0/temp1_input");
    file.open(QIODevice::ReadOnly);
    QString line = file.readLine();
    file.close();
    QString str(line);
    int lt = str.toDouble() * 0.001;

    file.setFileName("/sys/devices/system/cpu/cpufreq/policy0/scaling_cur_freq");
    file.open(QIODevice::ReadOnly);
    QString line1 = file.readLine();
    file.close();
    QString str1(line1);
    QString ln = QString::number(str1.toDouble() / 1000000, 'f', 2);

    file.setFileName("/proc/stat");
    file.open(QIODevice::ReadOnly);
    l = file.readLine();
    QByteArray ba;
    ba = l.toLatin1();
    const char *ch;
    ch = ba.constData();
    char cpu[5];
    long user,nice,sys,idle,iowait,irq,softirq,tt;
    sscanf(ch,"%s%ld%ld%ld%ld%ld%ld%ld",cpu,&user,&nice,&sys,&idle,&iowait,&irq,&softirq);
    tt = user + nice + sys + idle + iowait + irq + softirq;
    file.close();
    QString cusage = "";
    int cp = ((tt-tt0)-(idle-idle0))*100/(tt-tt0);
    if(i>0) cusage = "🏾  " + ln + " GHz  " + QString::number(lt) + " °C  " + QString::number(cp) + "%";
    idle0 = idle;
    tt0 = tt;

    // memory
    file.setFileName("/proc/meminfo");
    file.open(QIODevice::ReadOnly);
    l = file.readLine();
    long mt = l.replace("MemTotal:","").replace("kB","").replace(" ","").toLong();
    l = file.readLine();
    l = file.readLine();
    long ma = l.replace("MemAvailable:","").replace("kB","").replace(" ","").toLong();
    l = file.readLine();
    l = file.readLine();
    file.close();
    long mu = mt - ma;
    int mp = mu*100/mt;
    QString mem = "🝚  " + QString("%1 / %2 %3").arg(KB(mu)).arg(KB(mt)).arg(" " + QString::number(mp) + "%");

    // net
    file.setFileName("/proc/net/dev");
    file.open(QIODevice::ReadOnly);
    l = file.readLine();
    l = file.readLine();
    dbt1 = ubt1 = 0;
    while(!file.atEnd()){
        l = file.readLine();
        QStringList list = l.split(QRegExp("\\s{1,}"));
        db = list.at(1).toLong();
        ub = list.at(9).toLong();
        dbt1 += db;
        ubt1 += ub;
    }
    file.close();
    QString dss = "";
    QString uss = "";
    QString oss = "";
    if (i > 0) {
        ds = dbt1 - dbt0;
        us = ubt1 - ubt0;
        dss = NB(ds) + "/s";
        uss = NB(us) + "/s";
        if (ds > us) {
        oss = NB(ds) + "/s";
        } else {
        oss = NB(us) + "/s";
        }
        dbt0 = dbt1;
        ubt0 = ubt1;
    }
    QString net = "🡇  " + dss + "  " + BS(dbt1) + "\n🡅  " + uss + "  " + BS(ubt1);
    i++;
    if (i>2) i = 2;

    // draw
    m_tipsLabel->setText(startup + "\n" + uptime + "\n" + cusage + "\n" + mem + "\n" + net);
    m_centralWidget->text = oss;
    if (ds > 999) m_centralWidget->download = 1;
    else m_centralWidget->download = 0;
    if (us > 999) m_centralWidget->upload = 1;
    else m_centralWidget->upload = 0;
    m_centralWidget->update();

}

void NetspeedPlugin::bootRecord()
{
    QProcess *process = new QProcess;
    process->start("last reboot");
    process->waitForFinished();
    QString PO = process->readAllStandardOutput();
    QDialog *dialog = new QDialog;
    dialog->setWindowTitle(tr("Boot record"));
    dialog->setFixedSize(540,400);
    QVBoxLayout *vbox = new QVBoxLayout;
    QTextBrowser *textBrowser = new QTextBrowser;
    textBrowser->setText(PO);
    textBrowser->zoomIn();
    vbox->addWidget(textBrowser);
    QHBoxLayout *hbox = new QHBoxLayout;
    QPushButton *pushButton_confirm = new QPushButton("OK");
    hbox->addStretch();
    hbox->addWidget(pushButton_confirm);
    hbox->addStretch();
    vbox->addLayout(hbox);
    dialog->setLayout(vbox);
    dialog->show();
    connect(pushButton_confirm, SIGNAL(clicked()), dialog, SLOT(accept()));
    if(dialog->exec() == QDialog::Accepted){
        dialog->close();
    }
}

void NetspeedPlugin::bootAnalyze()
{
    QProcess *process = new QProcess;
    process->start("systemd-analyze blame");
    process->waitForFinished();
    QString PO = process->readAllStandardOutput();
    QDialog *dialog = new QDialog;
    dialog->setWindowTitle(tr("Boot analyze"));
    dialog->setFixedSize(540,400);
    QVBoxLayout *vbox = new QVBoxLayout;
    QTextBrowser *textBrowser = new QTextBrowser;
    textBrowser->setText(PO);
    textBrowser->zoomIn();
    vbox->addWidget(textBrowser);
    QHBoxLayout *hbox = new QHBoxLayout;
    QPushButton *pushButton_confirm = new QPushButton("OK");
    hbox->addStretch();
    hbox->addWidget(pushButton_confirm);
    hbox->addStretch();
    vbox->addLayout(hbox);
    dialog->setLayout(vbox);
    dialog->show();
    connect(pushButton_confirm, SIGNAL(clicked()), dialog, SLOT(accept()));
    if(dialog->exec() == QDialog::Accepted){
        dialog->close();
    }
}
