#ifndef QUERY_H
#define QUERY_H

#include "qbr_QueryGraphicsItem.h"
#include "qbr_QueryComponents.h"

#include <Qt>

class QueryGraphicsHelper;
class Table;
class SqlSourceEntity;

class Query : public QueryComponent
{
public:

    Query(QueryGraphicsHelper* components, int level, SqlSourceEntity* se) : QueryComponent(this, level), m_helper(components), m_graphicsItem(0), m_sqlSource(se) {}
    virtual ~Query() {}

    QueryGraphicsItem* getGraphicsItem()
    {
        return m_graphicsItem;
    }

    virtual bool initializeGraphicsItem() = 0;
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper*, QueryGraphicsItem*) = 0;
    virtual QVector<const Table*> getTables() const = 0;

    QueryGraphicsHelper* getHelper()
    {
        return m_helper;
    }
    SqlSourceEntity* getSourceEntity()
    {
        return m_sqlSource;
    }

    /** get returns the sql statement(s) of this query*/
    virtual QString get() const = 0;
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const = 0;

protected:
    QueryGraphicsHelper* m_helper;
    QueryGraphicsItem* m_graphicsItem;
    SqlSourceEntity* m_sqlSource;
};

#endif // QUERY_H
