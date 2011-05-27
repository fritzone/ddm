#ifndef SELECTQUERYORDERBYCOMPONENT_H
#define SELECTQUERYORDERBYCOMPONENT_H

#include "QueryComponents.h"

class SelectQueryOrderByComponent : public QueryComponent
{
public:
    SelectQueryOrderByComponent(QueryComponent*, int);
    virtual QString get(){return "ORDER BY"; }
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

#endif // SELECTQUERYORDERBYCOMPONENT_H
