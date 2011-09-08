#ifndef TABLEQUERYCOMPONENT_H
#define TABLEQUERYCOMPONENT_H

#include "qbr_QueryComponents.h"
#include "ColumnProviderForQuery.h"

class Table;
class SelectQueryAsComponent;
class TableGraphicsItem;
class SelectQueryJoinComponent;

class TableQueryComponent : public QueryComponent, ColumnProviderForQuery
{
public:

    static TableQueryComponent* provideFirstTableIfAny(QueryComponent* parent, int level);

public:
    TableQueryComponent(Table*,QueryComponent*,int);

    virtual QString get();
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*);
    virtual void handleAction(const QString& action, QueryComponent* referringObject);
    virtual QSet<OptionsType> provideOptions();
    virtual void onClose(){}
    virtual QueryComponent* duplicate();
    virtual QVector<CellTypeChooserType>  getTypeset() const
    {
        return QVector<CellTypeChooserType> ();
    }

    virtual QVector<const Column*> provideColumns();

    void removeAs();
    void removeJoin(SelectQueryJoinComponent*);
    void setTable(const QString& tab);
    const Table* getTable() const
    {
        return m_table;
    }

private:
    Table* m_table;
    SelectQueryAsComponent* m_as;
    QueryGraphicsHelper* m_helper;
    TableGraphicsItem* m_tgitm;
    QVector<SelectQueryJoinComponent*> m_joins;
};

#endif // TABLEQUERYCOMPONENT_H
