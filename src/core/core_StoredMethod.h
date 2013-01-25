#ifndef CORE_STOREDMETHOD_H
#define CORE_STOREDMETHOD_H

#include "NamedItem.h"
#include "SqlSourceTreeItem.h"
#include "TreeItem.h"
#include "SerializableElement.h"
#include "core_CloneableElement.h"
#include "core_ObjectWithUid.h"
#include "core_SqlSource.h"
#include "core_ParameterAndDescription.h"
#include "core_VersionElement.h"

class StoredMethod :
        public NamedItem,
        public SerializableElement,
        public CloneableElement,
        public VersionElement,
        public SqlSource,
        public TreeItem,
        public SqlSourceTreeItem
{
public:

    /**
     * Returns the keyword which is used in the SQL generation of this stored method (ie: FUNCTION/PROCEDURE)
     * @return the keyword used in SQL generation
     */
    virtual QString keyword() = 0;

    /**
     * @brief serialize
     * @param doc
     * @param parent
     * @see SerializableElement::serialize
     */
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const = 0;

    /**
     * @brief clone
     * @param sourceVersion
     * @param targetVersion
     * @return
     */
    virtual CloneableElement* clone(Version* sourceVersion, Version* targetVersion) = 0;

public:

    StoredMethod(const QString& name, const QString& uid, Version *v);

    virtual QStringList generateSqlSource(AbstractSqlGenerator*, QHash<QString,QString>, const Connection*);

    void setSql(const QString&);
    virtual void rename(const QString&);
    QString getSql() const;
    QVector<ParameterAndDescription> getParametersWithDescription();
    QString getBriefDescription() const
    {
        return m_brief;
    }
    QString getNameFromSql(int stidx, int& nameidx);
    QString getDescription() const
    {
        return m_desc;
    }
    QString getReturns() const
    {
        return m_returns;
    }

    virtual QString getSqlHash() const;


protected:

    QString m_sql;

    // these are populated automatically by the documentation generator
    QString m_brief;
    QString m_desc;
    QString m_returns;
};

#endif // CORE_STOREDMETHOD_H
