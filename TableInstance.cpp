#include "TableInstance.h"
#include "AbstractSQLGenerator.h"
#include "Table.h"
#include "Configuration.h"

TableInstance::TableInstance(Table *tab) : TreeItem(), NamedItem(tab->getName()), m_table(tab), m_values()
{
    for(int i=0; i<m_table->fullColumns().size(); i++)
    {
        m_values.insert(m_table->fullColumns()[i], QVector<QString>());
    }
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

QString TableInstance::generateSqlSource(AbstractSqlGenerator *generator, QHash<QString,QString> opts) const
{
    table()->restartSqlRendering();
    return generator->generateSql(this->table(), opts, getName());
}
