#ifndef CORE_PROCEDURE_H
#define CORE_PROCEDURE_H

#include "NamedItem.h"
#include "SqlSourceEntity.h"
#include "TreeItem.h"
#include "SerializableElement.h"

class Procedure : public SqlSourceEntity, public NamedItem, public TreeItem, public SerializableElement
{
public:
    Procedure();
    Procedure(const QString& pname);
    virtual QStringList generateSqlSource(AbstractSqlGenerator*, QHash<QString,QString>, const QString& codepage);
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;
    void setSql(const QString&);
    void rename(const QString&);
    QString getSql() const
    {
        return m_sql;
    }

private:
    QString m_sql;
};

#endif // CORE_PROCEDURE_H
