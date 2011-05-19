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
        // * small type choosers

        // ** used in a from query
        CELLTYPE_TABLE = 0,

        // ** used in a where condition
        CELLTYPE_NOT = 1,           // !
        CELLTYPE_MINUS = 2,         // -
        CELLTYPE_NEGATE = 3,        // ~
        CELLTYPE_PLUS = 4,          // +
        CELLTYPE_BINARY = 5,        // BINARY (!!) This is MySQL specific?
        CELLTYPE_EXISTS = 6,

        // * big type choosers
        // ** used in a wher condition
        CELLTYPE_COLUMN = 7,
        CELLTYPE_FUNCTION = 8,
        CELLTYPE_LITERAL = 9,
        CELLTYPE_QUERY = 10,

        CELLTYPE_NOTHING = 11

    };

    enum CellTypeChooserSize
    {
        CELLTYPECHOOSER_BIG = 0,
        CELLTYPECHOOSER_REGULAR = 1
    };

    CellTypeChooser(int level, CellTypeChooserSize size, CellTypeChooserType defaultType, QSet<CellTypeChooserType> allowedTypes, QueryGraphicsHelper*, QueryGraphicsItem*, QueryComponent* owner);
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
    CellTypeChooserSize m_size;
};

#endif // CELLTYPECHOOSER_H
