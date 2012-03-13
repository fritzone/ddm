#ifndef OBJECTWITHSPINSTANCES_H
#define OBJECTWITHSPINSTANCES_H

#include <QVector>
#include <QUuid>
#include <QMap>

#include <QDomDocument>
#include <QDomElement>

class DatabaseEngine;
class SpInstance;

class ObjectWithSpInstances
{
public:
    ObjectWithSpInstances();
    void initializeFor(const DatabaseEngine* engine, const QUuid& uid);
    QVector<SpInstance*>& getSpInstances(const DatabaseEngine* engine);
    void serialize_spinstances(QDomDocument& doc, QDomElement& parent) const;

private:
    QMap<const DatabaseEngine*, QVector<SpInstance*> > m_spInstances;

};

#endif // OBJECTWITHSPINSTANCES_H
