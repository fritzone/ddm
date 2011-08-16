#ifndef SELECTQUERYWHERECOMPONENT_H
#define SELECTQUERYWHERECOMPONENT_H

#include "qbr_QueryComponents.h"

class SelectQueryWhereComponent : public QueryComponent
{
public:

    // TODO: This is a classical lesson for polymorfism, I'm just too lazy right now
    enum WhereType
    {
        WHERETYPE_WHERE = 0,
        WHERETYPE_HAVING = 1,
        WHERETYPE_ON = 2
    };

    SelectQueryWhereComponent(QueryComponent*, int, WhereType);
    virtual QString get()
    {
        switch(m_whereType)
        {
        case WHERETYPE_WHERE:
            return "WHERE";
        case WHERETYPE_HAVING:
            return "HAVING";
        case WHERETYPE_ON:
            return "ON";
        }
    }
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*){return 0;}
    virtual void handleAction(const QString& action, QueryComponent* referringObject);
    virtual QSet<OptionsType> provideOptions();
    virtual void onClose(){}
    virtual QueryComponent* duplicate();
    virtual QVector<CellTypeChooserType>  getTypeset() const
    {
        return QVector<CellTypeChooserType> ();
    }

private:
    WhereType m_whereType;
};

#endif // SELECTQUERYWHERECOMPONENT_H
