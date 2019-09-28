#include "netspeedwidget.h"
#include "dde-dock/constants.h"
#include <QApplication>
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QLinearGradient>

#define PLUGIN_STATE_KEY    "enable"

NetspeedWidget::NetspeedWidget(QWidget *parent)
    : QWidget(parent),
      m_settings("deepin", "dde-dock-netspeed")

{
    text = "0 KB/s";
}

bool NetspeedWidget::enabled()
{
    return m_settings.value(PLUGIN_STATE_KEY, true).toBool();
}

void NetspeedWidget::setEnabled(const bool b)
{
    m_settings.setValue(PLUGIN_STATE_KEY, b);
}

QSize NetspeedWidget::sizeHint() const
{    
    QFont font = qApp->font();
    QFontMetrics FM(font);
    const Dock::Position position = qApp->property(PROP_POSITION).value<Dock::Position>();
    if (position == Dock::Top || position == Dock::Bottom)
        return QSize(FM.boundingRect(" 8.88 MB/s ").width() + 12, 26);
    else
        return QSize(FM.boundingRect("MB/s").width() + 12, FM.boundingRect("MB/s").height() * 2);
}

void NetspeedWidget::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
}

void NetspeedWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);

    QString iconName = "netspeed";
    const Dock::DisplayMode displayMode = qApp->property(PROP_DISPLAY_MODE).value<Dock::DisplayMode>();
    const Dock::Position position = qApp->property(PROP_POSITION).value<Dock::Position>();
    QPainter painter(this);

    if (displayMode == Dock::Efficient) {
        QFont font = qApp->font();
        painter.setFont(font);
        painter.setPen(Qt::white);
        if (position == Dock::Top || position == Dock::Bottom)
            painter.drawText(rect(), Qt::AlignRight | Qt::AlignVCenter, text + "  ");
        else
            painter.drawText(rect(), Qt::AlignCenter, text.replace(" ","\n"));
        return;
    }
    // position and size
    int side = qMin(width(), height());
    painter.translate(width() / 2, height() / 2);
    painter.scale(side / 200.0, side / 200.0);
    painter.setRenderHint(QPainter::Antialiasing);
    // background shadow
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 10));
    painter.drawRoundedRect(QRect(-71, -65, 142, 135), 14, 14);
    painter.setBrush(QColor(0, 0, 0, 30));
    painter.drawRoundedRect(QRect(-67, -61, 134, 127), 12, 12);
    // background
    painter.setBrush(QColor(255, 255, 255, 160));
    painter.drawRoundedRect(QRect(-66, -60, 132, 120), 8, 8);
    QLinearGradient m_gradient(0, -52, 0, 0);
    m_gradient.setColorAt(0, QColor(28, 122, 176));
    m_gradient.setColorAt(1, QColor(32, 88, 167));
    painter.setBrush(m_gradient);
    painter.drawRoundedRect(QRect(-58, -52, 116, 47), 4, 4);
    painter.drawRoundedRect(QRect(-58, 3, 116, 49), 4, 4);

    QColor dColor;
    QColor uColor;
    if (download == 0)
        dColor = Qt::transparent;
    else
        dColor = QColor(86, 214, 0);
    if (upload == 0)
        uColor = Qt::transparent;
    else
        uColor = QColor(255, 152, 0);
    painter.setPen(Qt::NoPen);
    painter.setBrush(dColor);
    painter.drawRect(-55, 3, 55, -7);
    painter.setBrush(uColor);
    painter.drawRect(0, 3, 55, -7);
    QFont font = qApp->font();
    QFontMetrics FM(font);
    font.setWeight(QFont::Black);
    font.setPixelSize(41);
    painter.setFont(font);
    QRect rectangle = QRect(-59, -53, 118, 106);
    painter.setPen(QColor(0, 0, 0, 70));
    painter.drawText(rectangle.adjusted (0, 2, 0, 0), Qt::AlignCenter, text.replace(" ","\n"));
    painter.setPen(Qt::white);
    painter.drawText(rectangle.adjusted (0, -4, 0, 0), Qt::AlignCenter, text.replace(" ","\n"));
}
