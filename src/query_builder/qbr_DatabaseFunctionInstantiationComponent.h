#ifndef QBR_DATABASEFUNCTIONINSTANTIATION_H
#define QBR_DATABASEFUNCTIONINSTANTIATION_H

#include "db_DatabaseBuiltinFunction.h"
#include "qbr_SingleExpressionQueryComponent.h"
#include "qbr_QueryComponents.h"
#include "core_SerializableElement.h"


class SingleExpressionQueryComponent;

/**
 * This class holds a reference to a database builtin function, and data about the
 * parametrization of that function, ie. how the stuff was populated in the query builder.
 */
class DatabaseFunctionInstantiationComponent : public SerializableElement
{
public:
    DatabaseFunctionInstantiationComponent(Query *q, QueryComponent* parent, const DatabaseBuiltinFunction& f, bool deserialized, Version *v);
    SingleExpressionQueryComponent* getInstantiatedParameter(int i);
    QString get();
    void addParameter(SingleExpressionQueryComponent* p)
    {
        m_parameters.append(p);
    }

    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;

private:
    QueryComponent* m_parent;
    const DatabaseBuiltinFunction& m_func;
    QVector<SingleExpressionQueryComponent*> m_parameters;
};

#endif // QBR_DATABASEFUNCTIONINSTANTIATION_H
