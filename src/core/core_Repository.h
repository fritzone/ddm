#ifndef CORE_REPOSITORY_H
#define CORE_REPOSITORY_H

#include <QVector>

class DatabaseEngine;
class Role;
class QDomElement;
class Entity;
class DBMenu;

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

    const QVector<Entity*>& getEntities() const
    {
        return m_entities;
    }

    DBMenu *createDbMenu(const QDomElement &elAction, DBMenu *&first);
private:

    Repository();
    void addDatabase(const QDomElement&);
    void addEntity(const QDomElement&);

private:
    QVector<DatabaseEngine*> m_databases;
    QVector<Role*> m_roles;
    QVector<Entity*> m_entities;
};

#endif // CORE_REPOSITORY_H
