#ifndef CELLTYPECHOOSER_H
#define CELLTYPECHOOSER_H

#include "qbr_QueryGraphicsItem.h"
#include "qbr_QueryGraphicsHelper.h"
#include "db_DatabaseBuiltinFunction.h"

#include <QSet>

class Column;

class CellTypeChooser : public QueryGraphicsItem
{
public:

    enum CellTypeChooserSize
    {
        CELLTYPECHOOSER_BIG = 0,
        CELLTYPECHOOSER_REGULAR = 1
    };

    CellTypeChooser(int level, CellTypeChooserSize size, CellTypeChooserType defaultType, QueryGraphicsHelper*, QueryGraphicsItem*, QueryComponent* owner );
    CellTypeChooser(int level, CellTypeChooserSize size, CellTypeChooserType defaultType, QSet<CellTypeChooserType> allowedTypes, QueryGraphicsHelper*, QueryGraphicsItem*, QueryComponent* owner);
    CellTypeChooser(int level, CellTypeChooserSize size, CellTypeChooserType defaultType, QSet<CellTypeChooserType> allowedTypes, QueryGraphicsHelper*, QueryGraphicsItem*, QueryComponent* owner, int index);

    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual void updateWidth(int newWidth);
    virtual void mousePress(int x, int y);
    virtual void mouseMove(int x, int y);
    virtual void mouseLeft(int x, int y);
    virtual void onClose(){}

    void setFunction(const DatabaseBuiltinFunction* f)
    {
        m_functionSupport = f;
    }

    const DatabaseBuiltinFunction* getFunction()
    {
        return m_functionSupport;
    }

    void setColumn(const Column* c);

    void setLiteral(const QString& s)
    {
        m_literal = s;
    }

private:
    CellTypeChooserType m_defaultType;
    QSet<CellTypeChooserType> m_allowedTypes;
    CellTypeChooserType m_currentType;
    QGraphicsRectItem* m_rect;
    CellTypeChooserSize m_size;
    int m_index;
    const DatabaseBuiltinFunction* m_functionSupport;
    QGraphicsTextItem* m_text;
    const Column* m_column;
    bool m_needsFrame;
    QString m_literal;
};

#endif // CELLTYPECHOOSER_H
