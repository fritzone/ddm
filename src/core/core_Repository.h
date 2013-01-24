#ifndef CORE_REPOSITORY_H
#define CORE_REPOSITORY_H

#include <QVector>

class DatabaseEngine;
class Role;
class QDomElement;

class Repository
{
public:

    static Repository* instance() ;
    ~Repository();

    const QVector<DatabaseEngine*> & getDatabases() const
    {
        return m_databases;
    }

    const QVector<Role*>& getRoles() const
    {
        return m_roles;
    }

private:

    Repository();
    void addDatabase(const QDomElement&);

private:
    QVector<DatabaseEngine*> m_databases;
    QVector<Role*> m_roles;
};

#endif // CORE_REPOSITORY_H
