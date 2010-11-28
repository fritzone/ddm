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

    QVector<QString> columns() const;

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

    virtual QStringList generateSqlSource(AbstractSqlGenerator *generator,QHash<QString,QString>, const QString& codepage);

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

    QString getReferencingTables() const
    {
        QString result = "";
        for(int i=0; i<m_referencingTables.size(); i++)
        {
            result += m_referencingTables.at(i)->getName();
            result += " ";
        }
        return result;
    }

    void addColumn(const QString& colName);

    QVector<QString> removeColumn(const QString& colName);

    void renameColumn(const QString& oldName, const QString& newName);

    void setSqlItem(ContextMenuEnabledTreeWidgetItem* sqlItem)
    {
        m_sqlItem = sqlItem;
    }

    void onDelete()
    {
        delete m_sqlItem;
    }

private:

    // the table of the table instance
    Table* m_table;

    // a hash in which to the column name we have mapped a list of default values.
    // TODO: This might need to be changed to be a hash of <Column*,QVector<QString> > instead ... think about it
    // TODO: for later versions come up with a solution that the USerDataType has values that can be put here ...
    // TODO: This "split" design for holding default values is simply horrible. Create a new class which holds default/startup values for the table and for the table
    // instances
    QHash < QString, QVector<QString> > m_values;

    // true if this table instance was created because of a reference (ie. foreign key for ex.)
    bool m_becauseOfReference;

    // this will contain a list of tables that have a foreign key to the table of this table instance
    QVector<Table*> m_referencingTables;

    // contains a list of other table instances that were created because this table instance's table required it (foreign key)
    QVector<TableInstance*> m_instantiatedTablesInstances;

    bool m_sentenced;

    // the SQL item if this table instance
    ContextMenuEnabledTreeWidgetItem* m_sqlItem;

};

#endif // TABLEINSTANCE_H
