#ifndef SELECTQUERYJOINCOMPONENT_H
#define SELECTQUERYJOINCOMPONENT_H

#include "QueryComponents.h"
#include "QueryGraphicsHelper.h"

class SelectQueryJoinComponent : public QueryComponent
{
public:
    SelectQueryJoinComponent(QueryComponent*, int);
    virtual QString get(){return "JOIN"; }
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*){return 0;}
    virtual void handleAction(const QString& action, QueryComponent* referringObject);
    virtual QSet<OptionsType> provideOptions();
    virtual void onClose();
    virtual QueryComponent* duplicate();
    virtual QSet<CellTypeChooserType>  getTypeset() const
    {
        return QSet<CellTypeChooserType> ();
    }

    void setHelper(QueryGraphicsHelper* h) {m_helper = h;}
private:
    QueryGraphicsHelper* m_helper;
};

#endif // SELECTQUERYJOINCOMPONENT_H
