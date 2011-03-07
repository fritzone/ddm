#ifndef SELECTQUERYWHERECOMPONENT_H
#define SELECTQUERYWHERECOMPONENT_H

#include "QueryComponents.h"

class SelectQueryWhereComponent : public QueryComponent
{
public:
    SelectQueryWhereComponent(QueryComponent*, int, bool);
    virtual QString get(){return m_trueWhere?"WHERE":"HAVING";}
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*){return 0;}
    virtual void handleAction(const QString& action);
    virtual QSet<OptionsType> provideOptions();
    virtual void onClose(){}
private:
    bool m_trueWhere;
};

#endif // SELECTQUERYWHERECOMPONENT_H
