#ifndef CORE_STOREDMETHOD_H
#define CORE_STOREDMETHOD_H

#include "NamedItem.h"
#include "SqlSourceEntity.h"
#include "TreeItem.h"
#include "SerializableElement.h"

class StoredMethod : public NamedItem, public SqlSourceEntity, public TreeItem, public SerializableElement
{
public:

    StoredMethod(const QString& name) : NamedItem(name) {}

    virtual QStringList generateSqlSource(AbstractSqlGenerator*, QHash<QString,QString>);
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const = 0;

    void setSql(const QString&);
    virtual void rename(const QString&);
    QString getSql() const;

protected:

    QString m_sql;
};

#endif // CORE_STOREDMETHOD_H
