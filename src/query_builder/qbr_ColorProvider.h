#ifndef QBR_COLORPROVIDER_H
#define QBR_COLORPROVIDER_H

#include <QColor>
#include <QBrush>

class ColorProvider
{
public:

    static QColor getTableCellBackgroundColor()
    {
        static QColor col(167, 186, 255);
        return col;
    }

    static QBrush getTableCellBackgroundBrush()
    {
        static QBrush brush(getTableCellBackgroundColor());
        return brush;
    }

    static QFont getfontForTableName()
    {
        static QFont arial14("Arial", 14, 2);
        return arial14;
    }

};

#endif // QBR_COLORPROVIDER_H
