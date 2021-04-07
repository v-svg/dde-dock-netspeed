#ifndef NetspeedPlugin_H
#define NetspeedPlugin_H

#include "dde-dock/pluginsiteminterface.h"
#include "netspeeditem.h"
#include "monitorwidget.h"
#include "floatingwidget.h"

#include <QTimer>
#include <QLabel>

class NetspeedPlugin : public QObject, PluginsItemInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginsItemInterface)
    Q_PLUGIN_METADATA(IID "com.deepin.dock.PluginsItemInterface" FILE "netspeed.json")

public:
     NetspeedPlugin(QObject *parent = nullptr);
    ~NetspeedPlugin();

    const QString pluginName() const override;
    const QString pluginDisplayName() const override;
    void init(PluginProxyInterface *proxyInter) override;
    QPixmap pixmap;
    void pluginStateSwitched() override;
    bool pluginIsAllowDisable() override { return true; }
    bool pluginIsDisable() override;

    int itemSortKey(const QString &itemKey);
    void setSortKey(const QString &itemKey, const int order);

    QWidget *itemWidget(const QString &itemKey) override;
    QWidget *itemTipsWidget(const QString &itemKey) override;
    QWidget *itemPopupApplet(const QString &itemKey) override;
    const QString itemContextMenu(const QString &itemKey) override;

    void invokedMenuItem(const QString &itemKey, const QString &menuId, const bool checked) override;

private slots:
    void updateNetspeed();

private:
    void bootRecord();
    void bootAnalyze();
    void showFloatingWidget();
    long int i, ds, us, db, ub, dbt1, ubt1, dbt0, ubt0, tt0, idle0;
    QLabel *m_tipsLabel;
    QTimer *m_refershTimer;
    QSettings m_settings;
    QString KB(long k);
    QString BS(long b);
    QString NB(long b);
    QString FB(long b);
    QString startup;
    NetspeedItem *m_netspeedItem;
    MonitorWidget *m_monitorWidget;
    FloatingWidget *m_floatingWidget;
};

#endif // NetspeedPlugin_H
