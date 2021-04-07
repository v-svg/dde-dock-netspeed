#ifndef NETSPEEDITEM_H
#define NETSPEEDITEM_H

#include <QWidget>
#include <QSettings>

class NetspeedItem : public QWidget
{
    Q_OBJECT

public:
    explicit NetspeedItem(QWidget *parent = nullptr);
    bool enabled();
    void setEnabled(const bool b);
    QString text;
    int download;
    int upload;

signals:
    void requestUpdateGeometry() const;
    void mouseMidBtnClicked() const;

protected:
    QSize sizeHint() const;
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private:
    QSettings m_settings;
    QString curText;
};

#endif // NETSPEEDITEM_H