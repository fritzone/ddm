#ifndef CORE_PARAMETERANDDESCRIPTION_H
#define CORE_PARAMETERANDDESCRIPTION_H

#include <QString>

struct ParameterAndDescription
{
    QString m_parameter;
    QString m_type;
    QString m_description;
    QString m_direction;
    int m_source; // 0 - doc, 1 - parameter list, 2 - verified in both
};

#endif // CORE_PARAMETERANDDESCRIPTION_H
