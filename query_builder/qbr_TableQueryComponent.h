#ifndef TABLEQUERYCOMPONENT_H
#define TABLEQUERYCOMPONENT_H

#include "qbr_QueryComponents.h"
#include "ColumnProviderForQuery.h"

class Table;
class TableInstance;
class SelectQueryAsComponent;
class TableGraphicsItem;
class SelectQueryJoinComponent;

class TableQueryComponent : public QueryComponent, ColumnProviderForQuery
{
public:

    static TableQueryComponent* provideFirstTableIfAny(QueryComponent* parent, int level);

public:
    TableQueryComponent(Table*, QueryComponent*, int, Version *v);
    TableQueryComponent(TableInstance* tinst, QueryComponent* p, int level, Version *v);

    virtual QString get() const;
    virtual QString getClass() const {return "TableQueryComponent";}
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*);
    virtual void handleAction(const QString& action, QueryComponent* );
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
    const Table* getTable() const;
    const TableInstance* getTableInstance() const;
    void setAs(SelectQueryAsComponent* as)
    {
        m_as = as;
    }
    void addJoin(SelectQueryJoinComponent* j)
    {
        m_joins.append(j);
    }

    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;
    virtual QUuid getClassUid() const;
    virtual CloneableElement* clone(Version* sourceVersion, Version* targetVersion);
private:
    // if m_table is 0 m_tinst is used
    Table* m_table;
    TableInstance* m_tinst;
    bool m_tabInsteadOfTinst;

    SelectQueryAsComponent* m_as;
    QueryGraphicsHelper* m_helper;
    TableGraphicsItem* m_tgitm;
    QVector<SelectQueryJoinComponent*> m_joins;
};

#endif // TABLEQUERYCOMPONENT_H
