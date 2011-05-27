#ifndef SELECTQUERYGROUPBYCOMPONENT_H
#define SELECTQUERYGROUPBYCOMPONENT_H

#include "QueryComponents.h"

class SelectQueryGroupByComponent : public QueryComponent
{
public:
    SelectQueryGroupByComponent(QueryComponent*, int);
    virtual QString get(){return "GROUP BY"; }
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*){return 0;}
    virtual void handleAction(const QString& action, QueryComponent* referringObject);
    virtual QSet<OptionsType> provideOptions();
    virtual void onClose(){}
    virtual QueryComponent* duplicate();
    virtual QSet<CellTypeChooserType>  getTypeset() const
    {
        return QSet<CellTypeChooserType> ();
    }

};

#endif // SELECTQUERYGROUPBYCOMPONENT_H
