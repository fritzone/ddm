#ifndef CORE_VIEW_H
#define CORE_VIEW_H

#include "NamedItem.h"
#include "SqlSourceEntity.h"
#include "TreeItem.h"
#include "SerializableElement.h"
#include "core_ObjectWithUid.h"

class SelectQuery;
class QueryGraphicsHelper;
class Version;

class View : virtual public SqlSourceEntity, virtual public NamedItem, virtual public TreeItem, virtual public SerializableElement, virtual public ObjectWithUid
{
public:
    View(bool manual, QString uid);
    View(Version* v, bool manual, QString uid); // used by the deserialization engine

    virtual QStringList generateSqlSource(AbstractSqlGenerator*, QHash<QString,QString>, const QString& codepage);

    QueryGraphicsHelper* getHelper() const
    {
        return m_helper;
    }

    SelectQuery* getQuery() const
    {
        return m_selectQuery;
    }

    void setQuery(SelectQuery* q)
    {
        m_selectQuery = q;
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

    void finalizeViewDeserialization();

    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;

    QString getManualSql() const
    {
        return m_sql;
    }

    QStringList getColumnNames() const
    {
        return m_columNames;
    }

    virtual QUuid getClassUid() const;

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
