#ifndef CORE_STOREDMETHOD_H
#define CORE_STOREDMETHOD_H

#include "NamedItem.h"
#include "SqlSourceEntity.h"
#include "TreeItem.h"
#include "SerializableElement.h"
#include "core_CloneableElement.h"

class StoredMethod : public NamedItem, public SqlSourceEntity, public TreeItem, public SerializableElement, public CloneableElement
{
public:

    struct ParameterAndDescription
    {
        QString m_parameter;
        QString m_type;
        QString m_description;
        QString m_direction;
        int m_source; // 0 - doc, 1 - parameter list, 2 - verified in both
    };


    StoredMethod(const QString& name) : NamedItem(name), m_sql(), m_brief() {}

    virtual QStringList generateSqlSource(AbstractSqlGenerator*, QHash<QString,QString>, const Connection*);
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const = 0;
    virtual QString keyword() = 0;
    virtual CloneableElement* clone(Version* sourceVersion, Version* targetVersion) = 0;

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

protected:

    QString m_sql;

    // these are populated automatically by the documentation generator
    QString m_brief;
    QString m_desc;
    QString m_returns;
};

#endif // CORE_STOREDMETHOD_H
