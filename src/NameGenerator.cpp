#include "NameGenerator.h"
#include "Version.h"

QString NameGenerator::getUniqueName(Version* v, itemGetter fun, const QString& prefix)
{
    QString result2 = prefix;
    if((v->*fun)(result2) == 0)
    {
        return result2;
    }

    int i = 1;
    while(i < MAX_IT)
    {
        QString result2 = prefix + "_" + QString::number(i);
        if((v->*fun)(result2) == 0)
        {
            return result2;
        }
        i++;
    }
    i = 1;
    while(i < MAX_IT)
    {
        QString result2 = "_" + prefix + "_" + QString::number(i);
        if((v->*fun)(result2) == 0)
        {
            return result2;
        }
        i++;
    }
    return "ERROR";
}

