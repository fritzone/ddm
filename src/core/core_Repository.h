#ifndef CORE_REPOSITORY_H
#define CORE_REPOSITORY_H

#include <QVector>

class DatabaseEngine;
class QDomElement;

class Repository
{
public:
    Repository();
    const QVector<DatabaseEngine*> & getDatabases() const
    {
        return m_databases;
    }
private:

    void addDatabase(const QDomElement&);

private:
    QVector<DatabaseEngine*> m_databases;
};

#endif // CORE_REPOSITORY_H
