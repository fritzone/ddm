#ifndef CELLTYPECHOOSER_H
#define CELLTYPECHOOSER_H

#include "QueryGraphicsItem.h"
#include "QueryComponents.h"

#include <QSet>

class CellTypeChooser : public QueryGraphicsItem
{
public:
    enum CellTypeChooserType
    {
        CELLTYPE_TABLE = 0
    };

    CellTypeChooser(CellTypeChooserType defaultType, QSet<CellTypeChooserType> allowedTypes, QueryComponents*);
    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual void updateWidth(int newWidth);
    virtual void mousePress(int x, int y){};
    virtual void mouseMove(int x, int y){};
    virtual void mouseLeft(int x, int y){};
private:
    CellTypeChooserType m_defaultType;
    QSet<CellTypeChooserType> m_allowedTypes;
    CellTypeChooserType m_currentType;
    QueryComponents* m_comps;
};

#endif // CELLTYPECHOOSER_H
