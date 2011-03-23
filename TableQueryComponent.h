#ifndef TABLEQUERYCOMPONENT_H
#define TABLEQUERYCOMPONENT_H

#include "QueryComponents.h"

class Table;
class SelectQueryAsComponent;

class TableQueryComponent : public QueryComponent
{
public:
    TableQueryComponent(Table*,QueryComponent*,int);
    virtual QString get();
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*);
    void setTable(const QString& tab);
    virtual void handleAction(const QString& action, QueryComponent* referringObject);
    virtual QSet<OptionsType> provideOptions();
    virtual void onClose(){}
    virtual QueryComponent* duplicate();
private:
    Table* m_table;
    SelectQueryAsComponent* m_as;
};

#endif // TABLEQUERYCOMPONENT_H
