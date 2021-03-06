#include "qbr_DatabaseFunctionInstantiationComponent.h"

DatabaseFunctionInstantiationComponent::DatabaseFunctionInstantiationComponent(Query* q,
                                                                               QueryComponent* parent,
                                                                               const DatabaseBuiltinFunction &f,
                                                                               bool deserialized, Version* v) :
    m_parent(parent), m_func(f)
{
    if(!deserialized)
    {
        for(int i=0; i<f.getParameterCount(); i++)
        {
            m_parameters.append(new SingleExpressionQueryComponent(q,
                                           m_parent,
                                           QueryComponent::PARAMETER_LEVEL, v));
        }
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

void DatabaseFunctionInstantiationComponent::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement funcInstElement = doc.createElement("Function");
    funcInstElement.setAttribute("Name", m_func.getName());
    QDomElement parametersElement = doc.createElement("Parameters");
    for(int i=0; i<m_parameters.size(); i++)
    {
        QDomElement parameterElement = doc.createElement("Parameter");
        parameterElement.setAttribute("idx", i);
        m_parameters.at(i)->serialize(doc, parameterElement);
        parametersElement.appendChild(parameterElement);
    }
    funcInstElement.appendChild(parametersElement);
    parent.appendChild(funcInstElement);
}
