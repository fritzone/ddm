#include "Table.h"

Table::Table() : m_columns()
{
}

void Table::addColumn(Column *column)
{
    m_columns.append(column);
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
        m_columns[c+1] = tc;
    }
}

const Column* Table::getColumn(int c)
{
    return m_columns[c];
}
