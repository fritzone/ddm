#ifndef SELECTQUERYGROUPBYCOMPONENT_H
#define SELECTQUERYGROUPBYCOMPONENT_H

#include "QueryComponents.h"

class SelectQueryGroupByComponent : public QueryComponent
{
public:
    SelectQueryGroupByComponent(QueryComponent*, int);
    virtual QString get(){return "WHERE"; }
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*){return 0;}
    virtual void handleAction(const QString& action);
    virtual QSet<OptionsType> provideOptions();
    virtual void onClose(){}
};

#endif // SELECTQUERYGROUPBYCOMPONENT_H
