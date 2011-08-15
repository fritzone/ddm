#ifndef QBR_DATABASEFUNCTIONINSTANTIATION_H
#define QBR_DATABASEFUNCTIONINSTANTIATION_H

#include "DatabaseBuiltinFunction.h"
#include "qbr_SingleExpressionQueryComponent.h"

/**
 * This class holds a reference to a database builtin function, and data about the
 * parametrization of that function, ie. how the stuff was populated in the query builder.
 */
class DatabaseFunctionInstantiationComponent
{
public:
    DatabaseFunctionInstantiationComponent(const DatabaseBuiltinFunction& f);

private:
    const DatabaseBuiltinFunction& m_func;
    QVector<SingleExpressionQueryComponent*> m_parameters;
};

#endif // QBR_DATABASEFUNCTIONINSTANTIATION_H
