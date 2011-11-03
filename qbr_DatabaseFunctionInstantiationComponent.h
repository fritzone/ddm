#ifndef QBR_DATABASEFUNCTIONINSTANTIATION_H
#define QBR_DATABASEFUNCTIONINSTANTIATION_H

#include "db_DatabaseBuiltinFunction.h"
#include "qbr_SingleExpressionQueryComponent.h"
#include "qbr_QueryComponents.h"
#include "SerializableElement.h"


class SingleExpressionQueryComponent;

/**
 * This class holds a reference to a database builtin function, and data about the
 * parametrization of that function, ie. how the stuff was populated in the query builder.
 */
class DatabaseFunctionInstantiationComponent : public SerializableElement
{
public:
    DatabaseFunctionInstantiationComponent(QueryComponent* parent, const DatabaseBuiltinFunction& f);
    SingleExpressionQueryComponent* getInstantiatedParameter(int i);
    QString get();
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;
private:
    QueryComponent* m_parent;
    const DatabaseBuiltinFunction& m_func;
    QVector<SingleExpressionQueryComponent*> m_parameters;
};

#endif // QBR_DATABASEFUNCTIONINSTANTIATION_H
