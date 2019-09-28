#ifndef NETSPEEDWIDGET_H
#define NETSPEEDWIDGET_H

#include <QWidget>
#include <QSettings>

class NetspeedWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NetspeedWidget(QWidget *parent = 0);
    bool enabled();
    void setEnabled(const bool b);
    QString text;
    int download;
    int upload;

signals:
    void requestUpdateGeometry() const;

private:
    QSize sizeHint() const;
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);
    QSettings m_settings;
};

#endif // NETSPEEDWIDGET_H