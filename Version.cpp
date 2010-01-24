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
