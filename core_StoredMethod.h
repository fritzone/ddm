#ifndef CORE_STOREDMETHOD_H
#define CORE_STOREDMETHOD_H

#include "NamedItem.h"
#include "SqlSourceEntity.h"
#include "TreeItem.h"
#include "SerializableElement.h"

class StoredMethod : public NamedItem, public SqlSourceEntity, public TreeItem, public SerializableElement
{
public:

    struct ParameterAndDescription
    {
        QString m_parameter;
        QString m_type;
        QString m_description;
        QString m_direction;
    };


    StoredMethod(const QString& name) : NamedItem(name), m_sql(), m_brief() {}

    virtual QStringList generateSqlSource(AbstractSqlGenerator*, QHash<QString,QString>, const Connection*);
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const = 0;
    virtual QString keyword() = 0;

    void setSql(const QString&);
    virtual void rename(const QString&);
    QString getSql() const;
    QVector<ParameterAndDescription> getParametersWithDescription();
    QString getBriefDescription() const
    {
        return m_brief;
    }
    QString getNameFromSql(int stidx, int& nameidx);

protected:

    QString m_sql;
    QString m_brief;

};

#endif // CORE_STOREDMETHOD_H
