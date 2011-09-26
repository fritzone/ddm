#include "qbr_DatabaseFunctionInstantiationComponent.h"

DatabaseFunctionInstantiationComponent::DatabaseFunctionInstantiationComponent(QueryComponent* parent, const DatabaseBuiltinFunction &f) : m_parent(parent), m_func(f)
{
    for(int i=0; i<f.getParameterCount(); i++)
    {
        m_parameters.append(new SingleExpressionQueryComponent(m_parent, -2));
    }
}

SingleExpressionQueryComponent* DatabaseFunctionInstantiationComponent::getInstantiatedParameter(int i)
{
    if(i >= m_parameters.size()) return 0;
    return m_parameters.at(i);
}

QString DatabaseFunctionInstantiationComponent::get()
{
    QString result = m_func.getNiceName();
    result += "(";
    for(int i=0; i<m_parameters.size(); i++)
    {
        result += m_parameters.at(i)->get();
        if(i < m_parameters.size() - 1) result += ", ";
    }
    result += ")";
    return result;
}
