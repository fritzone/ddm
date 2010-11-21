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
        QString result2 = prefix + " #" + QString::number(i);
        if(v->getTable(result2) == 0)
        {
            return result2;
        }
        i++;
    }
    i = 1;
    while(i < MAX_IT)
    {
        QString result2 = "_" + prefix + " #" + QString::number(i);
        if(v->getTable(result2) == 0)
        {
            return result2;
        }
        i++;
    }
    return "ERROR - NO MORE TABLES LEFT";
}


// TODO: These two below are ugly, duplicated code. Rewrite them somenhow, maybe with a method to member approach
QString NameGenerator::generateUniqueTableInstanceName(Version* v, QString input)
{
    int i = 1;
    while(i<MAX_IT)
    {
        QString result2 = input + " #" + QString::number(i);
        if(v->getTableInstance(result2) == 0)
        {
            return result2;
        }
        i++;
    }
    i = 1;
    while(i<MAX_IT)
    {
        QString result2 = "_" + input + " #" + QString::number(i);
        if(v->getTableInstance(result2) == 0)
        {
            return result2;
        }
        i++;
    }
    return input + "_copy";
}

QString NameGenerator::generateUniqueDiagramName(Version* v, QString input)
{
    int i = 1;
    while(i<MAX_IT)
    {
        QString result2 = input + " #" + QString::number(i);
        if(v->getDiagram(result2) == 0)
        {
            return result2;
        }
        i++;
    }
    i = 1;
    while(i<MAX_IT)
    {
        QString result2 = "_" + input + " #" + QString::number(i);
        if(v->getDiagram(result2) == 0)
        {
            return result2;
        }
        i++;
    }
    return input + "_copy";
}

QString NameGenerator::generateUniqueDatatypeName(Version* v, QString input)
{
    if(v->getDataType(input) == 0) return input;
    int i = 1;
    while(i<MAX_IT)
    {
        QString result2 = input + " #" + QString::number(i);
        if(v->getDataType(result2) == 0)
        {
            return result2;
        }
        i++;
    }
    i = 1;
    while(i<MAX_IT)
    {
        QString result2 = "_" + input + " #" + QString::number(i);
        if(v->getDataType(result2) == 0)
        {
            return result2;
        }
        i++;
    }
    return input + "_copy";
}
