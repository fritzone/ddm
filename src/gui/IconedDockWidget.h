#ifndef ICONEDDOCKWIDGET_H
#define ICONEDDOCKWIDGET_H

#include <QObject>
#include <QProxyStyle>
#include <QStyleOption>
#include <QPainter>

class IconedDockWidget: public QProxyStyle{
    Q_OBJECT
    QIcon icon_;
public:
    IconedDockWidget(const QIcon& icon,  QStyle* style = 0)
        : QProxyStyle(style)
        , icon_(icon)
    {}

    virtual ~IconedDockWidget()
    {}

    virtual void drawControl(ControlElement element, const QStyleOption* option,
        QPainter* painter, const QWidget* widget = 0) const
    {
        if(element == QStyle::CE_DockWidgetTitle)
        {
            //width of the icon
            int width = pixelMetric(QStyle::PM_ToolBarIconSize);
            //margin of title from frame
            int margin = baseStyle()->pixelMetric(QStyle::PM_DockWidgetTitleMargin);

            QPoint icon_point(margin + option->rect.left(),
                              margin + option->rect.center().y() - width/2);

            painter->drawPixmap(icon_point, icon_.pixmap(width, width));

            const_cast<QStyleOption*>(option)->rect = option->rect.adjusted(width, 0, 0, 0);
        }
        baseStyle()->drawControl(element, option, painter, widget);
    }
};


#endif // ICONEDDOCKWIDGET_H
