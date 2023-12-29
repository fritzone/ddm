#ifndef OBJECTWITHSPINSTANCES_H
#define OBJECTWITHSPINSTANCES_H

#include <QObject>
#include <QVector>
#include <QUuid>
#include <QMap>

#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

class DatabaseEngine;
class SpInstance;

class ObjectWithSpInstances : public QObject
{


public:

    void initializeFor(const DatabaseEngine* engine, const QUuid& uid);
    void initializeRemainingSps(const DatabaseEngine* engine, const QUuid& uid);
    QVector<SpInstance*>& getSpInstances(const DatabaseEngine* engine);
    void serialize_spinstances(QDomDocument& doc, QDomElement& parent) const;
    SpInstance* getInstance(const DatabaseEngine* eng, const QString& objectUid) const;
    SpInstance* getInstanceForSqlRoleUid(const DatabaseEngine* eng, const QString& sqlRoleUid) const;
    void addSpInstance(const DatabaseEngine* eng, SpInstance* spi);
    void clearSpInstances(const DatabaseEngine*);

    // copies the SPs over to the specified object
    void cloneSps(ObjectWithSpInstances* target);

private:
    void addSpInstance(const QString&, SpInstance*);

private:
    QMap<QString, QVector<SpInstance*> > m_spInstances;
};

#endif // OBJECTWITHSPINSTANCES_H
