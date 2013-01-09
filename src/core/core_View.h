#ifndef CORE_VIEW_H
#define CORE_VIEW_H

#include "NamedItem.h"
#include "SqlSourceTreeItem.h"
#include "TreeItem.h"
#include "SerializableElement.h"
#include "core_ObjectWithUid.h"
#include "core_CloneableElement.h"
#include "core_SqlSource.h"

class SelectQuery;
class QueryGraphicsHelper;
class Version;
class Table;

class View :
        public SqlSourceTreeItem,
        public NamedItem,
        public TreeItem,
        public SerializableElement,
        public ObjectWithUid,
        public CloneableElement,
        public SqlSource

{
public:
    View(Version *v, bool manual, QString uid);
    View(bool manual, QString uid, Version* v); // used by the deserialization engine

    virtual QStringList generateSqlSource(AbstractSqlGenerator*, QHash<QString,QString>, const Connection*);

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

    bool canReplace() const
    {
        return m_canReplace;
    }

    bool isManual() const
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

    virtual CloneableElement* clone(Version* sourceVersion, Version* targetVersion);
    void secondStepClone();

    virtual void updateGui();

    /**
     * @brief getHash
     * @return a hash of this Views's SQL in order for to be compared with other views
     */
    QString getSqlHash() const;
    QString getCreationStatement() const;
    QString getHashForCreationStatement() const;

    /**
     * Returns a list of tables the Query builder uses to fetch the data from... Only the first level.
     * @brief getSourceTables
     * @return
     */
    QVector<const Table *> getSourceTables() const;

    /**
     * Returns true if the view is using this table
     * @brief usesTable
     * @param tab
     * @return
     */
    bool usesTable(const Table* tab) const;

private:

    QString getHash(const QString&) const;

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
