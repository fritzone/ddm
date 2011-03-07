#ifndef CELLWHERECOMMAND_H
#define CELLWHERECOMMAND_H

#include "CellCommand.h"
class CellWhereCommand : public CellCommand
{
public:
    CellWhereCommand(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner, bool trueWhere);
    virtual QBrush getCellBrush();
    virtual QString getCommand()
    {
        return m_trueWhere?"WHERE":"HAVING";
    }
    virtual bool hasClose()
    {
        return true;
    }
    virtual CellQuerySmallOptionsBox* provideOptionsBox(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner)
    {
        return 0;
    }
    virtual void onClose();

private:
    bool m_trueWhere;
};

#endif // CELLWHERECOMMAND_H
