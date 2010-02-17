#include "Version.h"
#include "UserDataType.h"

Version::Version()
{
}

Version::~Version()
{
}

void Version::addNewDataType(UserDataType* dt)
{
    dataTypes.append(dt);
}


bool Version::hasDataType(const QString& name) const
{
    for(int i=0; i<dataTypes.size(); i++)
    {
        if(dataTypes.at(i)->getName() == name)
        {
            return true;
        }
    }
    return false;
}

UserDataType* Version::getDataType(const QString& name)
{
    for(int i=0; i<dataTypes.size(); i++)
    {
        if(dataTypes.at(i)->getName() == name)
        {
            return const_cast<UserDataType*>(dataTypes.at(i));  // yeah, this sucks
        }
    }
    return 0;
}

int Version::getDataTypeIndex(const QString& name)
{
    for(int i=0; i<dataTypes.size(); i++)
    {
        if(dataTypes.at(i)->getName() == name)
        {
            return i;
        }
    }
    return -1;
}
