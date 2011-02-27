#ifndef TABLEQUERYCOMPONENT_H
#define TABLEQUERYCOMPONENT_H

#include "QueryComponents.h"

class Table;

class TableQueryComponent : public QueryComponent
{
public:
    TableQueryComponent(Table*,QueryComponent*);
    virtual QString get();
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*);
    void setTable(const QString& tab);
    virtual void handleAction(const QString& action){}
    virtual QSet<OptionsType> provideOptions(){};
private:
    Table* m_table;
};

#endif // TABLEQUERYCOMPONENT_H
