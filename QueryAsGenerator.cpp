#include "QueryAsGenerator.h"
#include "QueryComponents.h"

QueryAsGenerator::QueryAsGenerator():m_forThis(0)
{}

QueryAsGenerator& QueryAsGenerator::instance()
{
    static QueryAsGenerator instance;
    return instance;
}

QString QueryAsGenerator::getNextValidAs()
{
    QString res = "T";
    int i=0;
    while(m_workingSet.contains(res))
    {
        i++;
        res = "T" + QString::number(i);
    }
    m_workingSet.insert(res);
    return res;
}
