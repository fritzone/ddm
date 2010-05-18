#include "Table.h"
#include "Column.h"
#include "Index.h"

Table::Table() : m_name(""), m_columns(), m_indices(), m_foreignKeys()
{
}

void Table::addColumn(Column *column)
{
    m_columns.append(column);
}

void Table::addIndex(Index *index)
{
    m_indices.append(index);
}

void Table::moveColumnDown(int c)
{
    if(c < m_columns.size() - 1)
    {
        Column* tc = m_columns[c];
        m_columns[c] = m_columns[c+1];
        m_columns[c+1] = tc;
    }
}

void Table::moveColumnUp(int c)
{
    if(c > 0)
    {
        Column* tc = m_columns[c];
        m_columns[c] = m_columns[c-1];
        m_columns[c-1] = tc;
    }
}

Column* Table::getColumn(int c)
{
    return m_columns[c];
}

Index* Table::getIndex(int c)
{
    return m_indices[c];
}

const Column* Table::getColumn(const QString& name)
{
    for(int i = 0; i<m_columns.size(); i++)
    {
        if(m_columns[i]->getName() == name)
        {
            return m_columns[i];
        }
    }

    return 0;
}

void Table::removeIndex(Index* toRemove)
{
    int idx = m_indices.indexOf(toRemove);
    if(idx != -1)
    {
        m_indices.remove(idx);
    }
}

void Table::removeColumn(Column* toRemove)
{
    int idx = m_columns.indexOf(toRemove);
    if(idx != -1)
    {
        m_columns.remove(idx);
    }
}

Index* Table::isColumnUsedInIndex(const Column *column)
{
    for(int i=0; i<m_indices.size(); i++)
    {
        if(m_indices[i]->hasColumn(column))
        {
            return m_indices[i];
        }
    }
    return 0;
}
