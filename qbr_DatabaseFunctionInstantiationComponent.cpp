#include "qbr_DatabaseFunctionInstantiationComponent.h"

DatabaseFunctionInstantiationComponent::DatabaseFunctionInstantiationComponent(const DatabaseBuiltinFunction &f) : m_func(f)
{
}

SingleExpressionQueryComponent* DatabaseFunctionInstantiationComponent::getInstantiatedParameter(int i)
{
    if(i > m_parameters.size()) return 0;
    return m_parameters.at(i);
}
