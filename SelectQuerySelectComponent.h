#ifndef SELECTQUERYSELECTCOMPONENT_H
#define SELECTQUERYSELECTCOMPONENT_H

#include "QueryComponents.h"

class SelectQuerySelectComponent : public QueryComponent
{
public:
    SelectQuerySelectComponent(QueryComponent*,int);
    virtual QString get(){return "SELECT"; }
    virtual void handleAction(const QString& action){}
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*){return 0;}
    virtual QSet<OptionsType> provideOptions();
    virtual void onClose();
};

#endif // SELECTQUERYSELECTCOMPONENT_H
