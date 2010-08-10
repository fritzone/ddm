#ifndef TABLEINSTANCE_H
#define TABLEINSTANCE_H

#include "Table.h"
#include "TreeItem.h"
#include "NamedItem.h"
#include "SerializableElement.h"
#include "SqlSourceEntity.h"

#include <QHash>
#include <QList>
#include <QString>

class TableInstance : public TreeItem, public NamedItem, public SerializableElement, public SqlSourceEntity
{
public:

    TableInstance(Table* tab);

    QList<QString> columns() const;

    Table* table() const
    {
        return m_table;
    }

    void setValues(QHash < QString, QVector<QString> > v)
    {
        m_values = v;
    }

    void serialize(QDomDocument &doc, QDomElement &parent) const;

    const QHash < QString, QVector<QString> >& values() const
    {
        return m_values;
    }

    virtual QStringList generateSqlSource(AbstractSqlGenerator *generator,QHash<QString,QString>) const;

private:

    Table* m_table;

    QHash < QString, QVector<QString> > m_values;

};

#endif // TABLEINSTANCE_H
