#ifndef SELECTQUERYFROMCOMPONENT_H
#define SELECTQUERYFROMCOMPONENT_H

#include "QueryComponents.h"

class SelectQueryFromComponent : public QueryComponent
{
public:
    SelectQueryFromComponent(QueryComponent*);
    virtual QString get(){return "FROM"; }
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*){return 0;}
    virtual void handleAction(const QString& action);
    virtual QSet<OptionsType> provideOptions();
};

#endif // SELECTQUERYFROMCOMPONENT_H
