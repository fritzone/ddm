#ifndef CELLTYPECHOOSER_H
#define CELLTYPECHOOSER_H

#include "QueryGraphicsItem.h"
#include "QueryGraphicsHelper.h"
#include "DatabaseBuiltinFunction.h"

#include <QSet>

class CellTypeChooser : public QueryGraphicsItem
{
public:

    enum CellTypeChooserSize
    {
        CELLTYPECHOOSER_BIG = 0,
        CELLTYPECHOOSER_REGULAR = 1
    };

    CellTypeChooser(int level, CellTypeChooserSize size, CellTypeChooserType defaultType, QSet<CellTypeChooserType> allowedTypes, QueryGraphicsHelper*, QueryGraphicsItem*, QueryComponent* owner);
    CellTypeChooser(int level, CellTypeChooserSize size, CellTypeChooserType defaultType, QSet<CellTypeChooserType> allowedTypes, QueryGraphicsHelper*, QueryGraphicsItem*, QueryComponent* owner, int index);

    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual void updateWidth(int newWidth);
    virtual void mousePress(int x, int y);
    virtual void mouseMove(int x, int y);
    virtual void mouseLeft(int x, int y);
    virtual void onClose(){}
private:
    CellTypeChooserType m_defaultType;
    QSet<CellTypeChooserType> m_allowedTypes;
    CellTypeChooserType m_currentType;
    QGraphicsRectItem* m_rect;
    CellTypeChooserSize m_size;
    int m_index;
    DatabaseBuiltinFunction* m_functionSupport;
};

#endif // CELLTYPECHOOSER_H
