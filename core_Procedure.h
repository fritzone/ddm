#ifndef CORE_PROCEDURE_H
#define CORE_PROCEDURE_H

#include "NamedItem.h"
#include "SqlSourceEntity.h"
#include "TreeItem.h"
#include "SerializableElement.h"

class Procedure : virtual public SqlSourceEntity, virtual public NamedItem, virtual public TreeItem, virtual public SerializableElement
{
public:
    Procedure();
    virtual QStringList generateSqlSource(AbstractSqlGenerator*, QHash<QString,QString>, const QString& codepage);
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const {}
    void setSql(const QString&);
    void rename(const QString&);

private:
    QString m_sql;
};

#endif // CORE_PROCEDURE_H
