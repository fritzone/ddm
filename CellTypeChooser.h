#ifndef CELLTYPECHOOSER_H
#define CELLTYPECHOOSER_H

#include "QueryGraphicsItem.h"
#include "QueryGraphicsHelper.h"

#include <QSet>

class CellTypeChooser : public QueryGraphicsItem
{
public:
    enum CellTypeChooserType
    {
        CELLTYPE_TABLE = 0
    };

    CellTypeChooser(CellTypeChooserType defaultType, QSet<CellTypeChooserType> allowedTypes, QueryGraphicsHelper*, QueryGraphicsItem*, QueryComponent* owner);
    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual void updateWidth(int newWidth);
    virtual void mousePress(int x, int y){};
    virtual void mouseMove(int x, int y);
    virtual void mouseLeft(int x, int y);
    virtual void onClose(){}
private:
    CellTypeChooserType m_defaultType;
    QSet<CellTypeChooserType> m_allowedTypes;
    CellTypeChooserType m_currentType;
    QGraphicsRectItem* m_rect;
};

#endif // CELLTYPECHOOSER_H
