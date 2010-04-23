#include "Index.h"
#include "Column.h"


Index::Index() : m_name(""), m_type(""), m_columns()
{
}

Index::Index(const QString &name, const QString &type) : m_name(name), m_type(type), m_columns()
{
}

void Index::addColumn(const Column* column)
{
    m_columns.append(column);
}

bool Index::hasColumn(const Column* column) const
{
    for(int i=0; i<m_columns.size(); i++)
    {
        if(m_columns[i] == column)
        {
            return true;
        }
    }
    return false;
}

const QString& Index::getName() const
{
    return m_name;
}

const QString& Index::getType() const
{
    return m_type;
}

void Index::setName(const QString &name)
{
    m_name = name;
}

void Index::setType(const QString &type)
{
    m_type = type;
}

void Index::resetColumns()
{
    m_columns.clear();
}
