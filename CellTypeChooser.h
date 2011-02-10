#ifndef CELLTYPECHOOSER_H
#define CELLTYPECHOOSER_H

#include "QueryGraphicsItem.h"
#include <QSet>

class CellTypeChooser : public QueryGraphicsItem
{
public:

    enum CellTypeChooserType
    {
        CELLTYPE_TABLE = 0
    };

    static const int CELL_SIZE = 16;

    CellTypeChooser(CellTypeChooserType defaultType, QSet<CellTypeChooserType> allowedTypes);
    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual void updateWidth(int newWidth){};

private:
    CellTypeChooserType m_defaultType;
    QSet<CellTypeChooserType> m_allowedTypes;
};

#endif // CELLTYPECHOOSER_H
