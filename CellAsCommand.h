#ifndef CELLASCOMMAND_H
#define CELLASCOMMAND_H

#include "CellCommand.h"

class QueryTextInputItem;

class CellAsCommand : public CellCommand
{
public:
    CellAsCommand(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner);
    virtual QBrush getCellBrush();
    virtual QString getCommand()
    {
        return "AS";
    }
    virtual bool hasClose()
    {
        return false;
    }
    virtual void mousePress(int x, int y);
    virtual void mouseMove(int x, int y);
    virtual void mouseLeft(int x, int y);
    virtual void onClose();
    virtual void updateWidth(int newWidth);
    QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual CellQuerySmallOptionsBox* provideOptionsBox(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner);
private:
    QGraphicsRectItem* m_textInputRect;
    QueryTextInputItem* m_textItem;
    QString m_strText;
};

#endif // CELLASCOMMAND_H
