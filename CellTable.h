#ifndef CELLTABLE_H
#define CELLTABLE_H

#include "Cell.h"
#include "QueryComponents.h"

class CellClose;
class CellTableChooser;

class CellTable : public Cell
{
public:
    enum { Type = UserType + 10006 };
    int type() const { return Type; }

    CellTable(const QString& tabName, QueryComponents*);
    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual void updateWidth(int newWidth);
    virtual void mousePress(int x, int y){};
    virtual void mouseMove(int x, int y){};
    virtual void mouseLeft(int x, int y){};
private:
    QString m_tabName;
    CellClose* m_closer;
    CellTableChooser* m_chooser;
    QGraphicsRectItem* m_frame;
    QueryComponents* m_comps;
};

#endif // CELLTABLE_H
