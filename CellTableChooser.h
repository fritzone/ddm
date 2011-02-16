#ifndef CELLTABLECHOOSER_H
#define CELLTABLECHOOSER_H

#include "QueryGraphicsItem.h"
#include "QueryComponents.h"

class CellTableChooser : public QueryGraphicsItem
{
public:
    CellTableChooser(const QString& name, QueryComponents*);
    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual void updateWidth(int newWidth);
    virtual void mousePress(int x, int y);
    virtual void mouseMove(int x, int y);
    virtual void mouseLeft(int x, int y);
private:
    QString m_name;
    QGraphicsRectItem* m_frame;
    QueryComponents* m_comps;
    QGraphicsTextItem* m_txt;
};

#endif // CELLTABLECHOOSER_H
