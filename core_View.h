#ifndef CORE_VIEW_H
#define CORE_VIEW_H

#include "NamedItem.h"
#include "SqlSourceEntity.h"
#include "TreeItem.h"
#include "SerializableElement.h"

class SelectQuery;
class QueryGraphicsHelper;
class Version;

class View : virtual public SqlSourceEntity, virtual public NamedItem, virtual public TreeItem, virtual public SerializableElement
{
public:
    View(bool manual);
    View(Version* v, bool manual); // used by the deserialization engine

    virtual QStringList generateSqlSource(AbstractSqlGenerator*, QHash<QString,QString>, const QString& codepage);

    QueryGraphicsHelper* getHelper() const
    {
        return m_helper;
    }

    SelectQuery* getQuery() const
    {
        return m_selectQuery;
    }

    void setColumnNames(const QStringList& list)
    {
        m_columNames = list;
    }

    void setReplace(bool b)
    {
        m_canReplace = b;
    }

    bool getReplace() const
    {
        return m_canReplace;
    }

    bool getManual() const
    {
        return m_manual;
    }

    void setSql(const QString& sql)
    {
        m_sql = sql;
    }

    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;

private:
    SelectQuery* m_selectQuery;
    QueryGraphicsHelper* m_helper;
    QStringList m_columNames;
    bool m_canReplace;
    // whether SQL for the view was manually typed in or the query builder was used
    bool m_manual;
    // if this is a manual query the m_sql is the SQL query
    QString m_sql;
};

#endif // CORE_VIEW_H
