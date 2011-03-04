#ifndef SELECTQUERYWHERECOMPONENT_H
#define SELECTQUERYWHERECOMPONENT_H

#include "QueryComponents.h"

class SelectQueryWhereComponent : public QueryComponent
{
public:
    SelectQueryWhereComponent(QueryComponent*, int);
    virtual QString get(){return "WHERE"; }
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*){return 0;}
    virtual void handleAction(const QString& action);
    virtual QSet<OptionsType> provideOptions();
    virtual void onClose(){}
};

#endif // SELECTQUERYWHERECOMPONENT_H
