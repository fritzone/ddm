#include "qbr_QueryAsGenerator.h"
#include "qbr_QueryComponents.h"

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

bool QueryAsGenerator::hasAs(const QString &as)
{
    return m_workingSet.contains(as);
}

void QueryAsGenerator::addAs(const QString &as)
{
    m_workingSet.insert(as);
}
