#include "Index.h"
#include "Column.h"


Index::Index() : m_name(""), m_type(""), m_columns()
{
}

Index::Index(const QString &name, const QString &type) : m_name(name), m_type(type), m_columns()
{
}

void Index::addColumn(const Column *column)
{
    m_columns.append(column);
}
