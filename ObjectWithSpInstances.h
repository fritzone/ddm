#ifndef OBJECTWITHSPINSTANCES_H
#define OBJECTWITHSPINSTANCES_H

#include <QVector>

class SpInstance;

class ObjectWithSpInstances
{
public:
    ObjectWithSpInstances();

private:
    QVector<SpInstance*> m_spInstances;

};

#endif // OBJECTWITHSPINSTANCES_H
