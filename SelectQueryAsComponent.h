#ifndef SELECTQUERYASCOMPONENT_H
#define SELECTQUERYASCOMPONENT_H

#include "QueryComponents.h"

class SelectQueryAsComponent : public QueryComponent
{
public:
    SelectQueryAsComponent(QueryComponent*, int);
    virtual QString get(){return "AS"; }
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*){return 0;}
    virtual void handleAction(const QString& action);
    virtual QSet<OptionsType> provideOptions();
    virtual void onClose(){}
    void setAs(const QString& as)
    {
        m_as = as;
    }

    QString getAs() const
    {
        return m_as;
    }

private:

    QString m_as;
};

#endif // SELECTQUERYASCOMPONENT_H
