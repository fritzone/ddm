#include "Version.h"
#include "UserDataType.h"
#include "Table.h"

Version::Version()
{
}

Version::~Version()
{
}

void Version::addNewDataType(UserDataType* dt)
{
    m_dataTypes.append(dt);
}


bool Version::hasDataType(const QString& name) const
{
    for(int i=0; i < m_dataTypes.size(); i++)
    {
        if(m_dataTypes.at(i)->getName() == name)
        {
            return true;
        }
    }
    return false;
}

UserDataType* Version::getDataType(const QString& name)
{
    for(int i=0; i< m_dataTypes.size(); i++)
    {
        if(m_dataTypes.at(i)->getName() == name)
        {
            return const_cast<UserDataType*>(m_dataTypes.at(i));  // yeah, this sucks
        }
    }
    return 0;
}

int Version::getDataTypeIndex(const QString& name)
{
    for(int i=0; i < m_dataTypes.size(); i++)
    {
        if(m_dataTypes.at(i)->getName() == name)
        {
            return i;
        }
    }
    return -1;
}

void Version::addTable(Table *t)
{
    m_tables.append(t);
}

bool Version::hasTable(Table *t)
{
    return m_tables.indexOf(t) >= 0;
}

Table* Version::getTable(const QString &name)
{
    for(int i=0; i< m_tables.size(); i++)
    {
        if(m_tables[i]->getName() == name)
        {
            return m_tables[i];
        }
    }
    return 0;
}
