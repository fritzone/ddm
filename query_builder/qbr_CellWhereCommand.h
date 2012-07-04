#ifndef CELLWHERECOMMAND_H
#define CELLWHERECOMMAND_H

#include "qbr_CellCommand.h"
#include "qbr_SelectQueryWhereComponent.h"

class CellWhereCommand : public CellCommand
{
public:
    CellWhereCommand(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner, SelectQueryWhereComponent::WhereType);
    virtual QBrush getCellBrush();
    virtual QString getCommand()
    {
        switch(m_whereType)
        {
        case SelectQueryWhereComponent::WHERETYPE_WHERE:
            return "WHERE";
        case SelectQueryWhereComponent::WHERETYPE_HAVING:
            return "HAVING";
        case SelectQueryWhereComponent::WHERETYPE_ON:
            return "ON";
        }

        return "?";
    }
    virtual bool hasClose()
    {
        switch(m_whereType)
        {
        case SelectQueryWhereComponent::WHERETYPE_WHERE:
        case SelectQueryWhereComponent::WHERETYPE_HAVING:
            return true;
        case SelectQueryWhereComponent::WHERETYPE_ON:
            return false;
        }

        return false;
    }

    virtual CellQuerySmallOptionsBox* provideOptionsBox(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner);
    virtual void onClose();
    virtual bool hasTypeChooser() {return false;}
    SelectQueryWhereComponent::WhereType getType() const
    {
        return m_whereType;
    }

private:
    SelectQueryWhereComponent::WhereType m_whereType;
};

#endif // CELLWHERECOMMAND_H
