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

    TableInstance(Table* tab, bool ref);

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

    bool instantiatedBecuaseOfRkReference() const
    {
        return m_becauseOfReference;
    }

    void addTableReferencingThis(Table* refTab);

    void addInstantiatedTableInstance(TableInstance* tabInst)
    {
        m_instantiatedTablesInstances.append(tabInst);
    }

    virtual QStringList generateSqlSource(AbstractSqlGenerator *generator,QHash<QString,QString>);

    QVector<TableInstance*>& getInstantiatedTableInstances()
    {
        return m_instantiatedTablesInstances;
    }

    void sentence()
    {
        m_sentenced = true;
    }

    bool sentenced() const
    {
        return m_sentenced;
    }

private:

    Table* m_table;

    // a hash in which to the column name we have mapped a list of default values.
    QHash < QString, QVector<QString> > m_values;

    bool m_becauseOfReference;

    QVector<Table*> m_referencingTables;

    QVector<TableInstance*> m_instantiatedTablesInstances;

    bool m_sentenced;

};

#endif // TABLEINSTANCE_H
