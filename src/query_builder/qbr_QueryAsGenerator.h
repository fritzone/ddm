#ifndef QUERYASGENERATOR_H
#define QUERYASGENERATOR_H

#include <QString>
#include <QSet>
#include <QMap>

class QueryComponent;

class QueryAsGenerator
{
public:
    QString getNextValidAs();
    bool hasAs(const QString& as);
    void addAs(const QString& as);

    void initNewQuery(QueryComponent* forThis)
    {
        if(m_asList.contains(forThis))
        {
            m_asList[m_forThis] = m_workingSet;
            m_workingSet = m_asList[forThis];
        }
        else
        {
            m_workingSet = QSet<QString>();
            m_asList.insert(forThis, m_workingSet);
        }
        m_forThis = forThis;
    }


    static QueryAsGenerator& instance();


private:
    QueryAsGenerator();

private:
    QMap<QueryComponent*, QSet<QString> > m_asList;
    QSet<QString> m_workingSet;
    QueryComponent* m_forThis;
};

#endif // QUERYASGENERATOR_H
