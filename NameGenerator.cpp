#include "NameGenerator.h"
#include "Version.h"

NameGenerator::NameGenerator()
{
}

QString NameGenerator::getNextTableNameFromVersion(Version* v, QString prefix)
{
    QString result2 = prefix;
    if(v->getTable(result2) == 0)
    {
        return result2;
    }

    int i = 1;
    while(i < MAX_IT)
    {
        QString result2 = prefix + "_" + QString::number(i);
        if(v->getTable(result2) == 0)
        {
            return result2;
        }
        i++;
    }
    i = 1;
    while(i < MAX_IT)
    {
        QString result2 = "_" + prefix + "_" + QString::number(i);
        if(v->getTable(result2) == 0)
        {
            return result2;
        }
        i++;
    }
    return "ERROR - NO MORE TABLES LEFT";
}
