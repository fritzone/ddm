#ifndef CELLTABLE_H
#define CELLTABLE_H

#include "QueryGraphicsHelper.h"
#include "QueryGraphicsItem.h"

class CellClose;
class CellTableChooser;

class CellTable : public QueryGraphicsItem
{
public:
    CellTable(const QString& tabName, QueryGraphicsHelper*, QueryGraphicsItem* parent, QueryComponent* owner);
    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual void updateWidth(int newWidth);
    virtual void mousePress(int x, int y){}
    virtual void mouseMove(int x, int y){}
    virtual void mouseLeft(int x, int y){}
    virtual void onClose();
private:
    QString m_tabName;
    CellClose* m_closer;
    CellTableChooser* m_chooser;
    QGraphicsRectItem* m_frame;
};

#endif // CELLTABLE_H
