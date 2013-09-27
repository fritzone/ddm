#ifndef CORE_VIEW_H
#define CORE_VIEW_H

#include "core_NamedItem.h"
#include "SqlSourceTreeItem.h"
#include "TreeItem.h"
#include "core_SerializableElement.h"
#include "core_ObjectWithUid.h"
#include "core_CloneableElement.h"
#include "core_SqlSource.h"
#include "core_VersionElement.h"
#include "ObjectWithSpInstances.h"

class SelectQuery;
class QueryGraphicsHelper;
class Version;
class Table;
class TableInstance;

class View :
        public SqlSourceTreeItem,
        public NamedItem,
        public TreeItem,
        public SerializableElement,
        public VersionElement,
        public ObjectWithSpInstances,
        public CloneableElement,
        public SqlSource

{
public:
    explicit View() {}

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

    QVector<const TableInstance *> getSourceTableInstances() const;

    /**
     * Returns true if the view is using this table
     * @brief usesTable
     * @param tab
     * @return
     */
    bool usesTable(const Table* tab) const;
    bool usesTableInstance(const TableInstance* tab) const;

    static const QVector<View*>& family(const View&);


private:

    QString getHash(const QString&) const;

private:
    SelectQuery* m_selectQuery;
    QueryGraphicsHelper* m_helper;
    QStringList m_columNames;

    // whether SQL for the view was manually typed in or the query builder was used
    bool m_manual;
    // if this is a manual query the m_sql is the SQL query
    QString m_sql;
};

#endif // CORE_VIEW_H
