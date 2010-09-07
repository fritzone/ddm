#include "TableInstance.h"
#include "AbstractSQLGenerator.h"
#include "Table.h"
#include "Configuration.h"

TableInstance::TableInstance(Table *tab, bool ref) : TreeItem(), NamedItem(tab->getName()), m_table(tab), m_values(), m_becauseOfReference(ref), m_instantiatedTablesInstances(), m_referencingTables(0), m_sentenced(false)
{
    for(int i=0; i<m_table->fullColumns().size(); i++)
    {
        m_values.insert(m_table->fullColumns()[i], QVector<QString>());
    }
    tab->addInstance(this);
}

void TableInstance::addTableReferencingThis(Table* refTab)
{
    m_referencingTables.append(refTab);
}

QList<QString> TableInstance::columns() const
{
    QList<QString> v = m_values.keys();
    return v;
}

void TableInstance::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement tableInstanceElement = doc.createElement("TableInstance");      // will hold the data in this element
    tableInstanceElement.setAttribute("Name", getName());
    tableInstanceElement.setAttribute("Table", m_table->getName());
    tableInstanceElement.setAttribute("Ref", instantiatedBecuaseOfRkReference());
    {
    QString refTables = "";
    for(int i=0; i<m_referencingTables.size(); i++)
    {
        refTables += m_referencingTables.at(i)->getName();
        if(i<m_referencingTables.size()-1)
        {
            refTables += ",";
        }
    }
    tableInstanceElement.setAttribute("ReferencingTables", refTables);
    }

    {
    QString instantiatedTablesInstances = "";
    for(int i=0; i<m_instantiatedTablesInstances.size(); i++)
    {
        instantiatedTablesInstances += m_instantiatedTablesInstances.at(i)->getName();
        if(i<m_instantiatedTablesInstances.size()-1)
        {
            instantiatedTablesInstances += ",";
        }
    }
    tableInstanceElement.setAttribute("InstantiatedTableInstances", instantiatedTablesInstances);
    }

    QList <QString> cols = columns();

    for(int i=0; i<cols.size(); i++)
    {
        QDomElement column = doc.createElement("Column");
        column.setAttribute("Name", cols[i]);
        QVector<QString> colVs = m_values[cols[i]];
        for(int j=0; j<colVs.size(); j++)
        {
            QDomElement value = doc.createElement("Value");
            value.setAttribute("value", colVs[j]);
            column.appendChild(value);
        }
        tableInstanceElement.appendChild(column);
    }

    parent.appendChild(tableInstanceElement);
}

QStringList TableInstance::generateSqlSource(AbstractSqlGenerator *generator, QHash<QString,QString> opts)
{
    QStringList result;
    table()->restartSqlRendering();
    result << generator->generateCreateTableSql(this->table(), opts, getName());
    // and now the default values as "inserts"
    result << generator->generateDefaultValuesSql(this, opts);
    return result;
}

void TableInstance::addColumn(const QString& colName)
{
    m_values.insert(colName, QVector<QString>());
}

QVector<QString> TableInstance::removeColumn(const QString &colName)
{
    QVector<QString> result = m_values[colName];
    m_values.erase(m_values.find(colName));
    return result;
}

void TableInstance::renameColumn(const QString& oldName, const QString& newName)
{
    QVector<QString> tmp = removeColumn(oldName);
    m_values.insert(newName, tmp);
}
