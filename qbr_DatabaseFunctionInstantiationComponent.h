#ifndef QBR_DATABASEFUNCTIONINSTANTIATION_H
#define QBR_DATABASEFUNCTIONINSTANTIATION_H

#include "DatabaseBuiltinFunction.h"
#include "qbr_SingleExpressionQueryComponent.h"
#include "qbr_QueryComponents.h"

class SingleExpressionQueryComponent;

/**
 * This class holds a reference to a database builtin function, and data about the
 * parametrization of that function, ie. how the stuff was populated in the query builder.
 */
class DatabaseFunctionInstantiationComponent
{
public:
    DatabaseFunctionInstantiationComponent(QueryComponent* parent, const DatabaseBuiltinFunction& f);
    SingleExpressionQueryComponent* getInstantiatedParameter(int i);

private:
    QueryComponent* m_parent;
    const DatabaseBuiltinFunction& m_func;
    QVector<SingleExpressionQueryComponent*> m_parameters;
};

#endif // QBR_DATABASEFUNCTIONINSTANTIATION_H
