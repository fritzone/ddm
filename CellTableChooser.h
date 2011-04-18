#ifndef CELLTABLECHOOSER_H
#define CELLTABLECHOOSER_H

#include "QueryGraphicsItem.h"
#include "QueryGraphicsHelper.h"

class CellTableChooser : public QueryGraphicsItem
{
public:
    CellTableChooser(const QString& name, int level, QueryGraphicsHelper*,QueryGraphicsItem*, QueryComponent* owner);
    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual void updateWidth(int newWidth);
    virtual void mousePress(int x, int y);
    virtual void mouseMove(int x, int y);
    virtual void mouseLeft(int x, int y);
    virtual void onClose(){}
private:
    QString m_name;
    QGraphicsRectItem* m_frame;
    QGraphicsTextItem* m_txt;
};

#endif // CELLTABLECHOOSER_H
