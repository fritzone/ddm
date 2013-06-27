#ifndef QUERY_H
#define QUERY_H

#include "qbr_QueryGraphicsItem.h"
#include "qbr_QueryComponents.h"

#include <Qt>

class QueryGraphicsHelper;
class Table;
class SqlSource;
class TableInstance;
class SelectQueryAsComponent;
class TableQueryComponent;

class Query : public QueryComponent
{
public:

    Query(QueryGraphicsHelper* components, int level, SqlSource* se, Version *v) :
        QueryComponent(this, this, level, v),
        m_helper(components), m_graphicsItem(0), m_sqlSource(se)
    {}

    virtual ~Query() {}

    QueryGraphicsItem* getGraphicsItem()
    {
        return m_graphicsItem;
    }

    virtual bool initializeGraphicsItem() = 0;
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*) = 0;

    virtual QVector<const Table*> getTables() const = 0;
    virtual QVector<const TableInstance*> getTableInstances() const = 0;


    QueryGraphicsHelper* getHelper()
    {
        return m_helper;
    }
    SqlSource* getSourceEntity()
    {
        return m_sqlSource;
    }

    /** get returns the sql statement(s) of this query*/
    virtual QString get() const = 0;
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const = 0;
    virtual QUuid getClassUid() const = 0;

    /**
     * @brief tableRemovedFromQuery called when a table was removed from the query
     * in order to remove the components that were referencing it
     * @param tabName
     */
    virtual void tableRemovedFromQuery(const QString& tabName) = 0;

    void setQueryGlobalAlias(const SelectQueryAsComponent* as, const TableQueryComponent* tab);
    void deleteQueryGlobalAlias(const TableQueryComponent *tab);
    QString applyAlias(const QString &t);


protected:
    QueryGraphicsHelper* m_helper;
    QueryGraphicsItem* m_graphicsItem;
    SqlSource* m_sqlSource;

    QMap<const TableQueryComponent*, const SelectQueryAsComponent*> m_queryAliases;

};

#endif // QUERY_H
