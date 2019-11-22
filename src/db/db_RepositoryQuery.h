#ifndef DB_REPOSITORYQUERY_H
#define DB_REPOSITORYQUERY_H

#include <QtXml/QDomElement>
#include <QMap>
#include <QString>

class Connection;

/**
 * @brief The RepositoryQuery class is a query loaded from the repository
 */
class RepositoryQuery
{
public:

    RepositoryQuery(const QDomElement&);

    QString render(Connection *c);

    QString getId() const
    {
        return m_id;
    }

    int getResultIndex() const
    {
        return m_resultIndex;
    }

private:

    QString m_id;
    int m_resultIndex;
    QMap<int, QString> m_parts;
};

#endif // DB_REPOSITORYQUERY_H
