#ifndef CORE_PARAMETERANDDESCRIPTION_H
#define CORE_PARAMETERANDDESCRIPTION_H

#include "TreeItem.h"
#include <QString>

enum PARAMETER_SOURCE
{
    DOC = 0,
    PARAM_LIST = 1,
    VER_IN_BOTH = 2,
    GUIDED = 3,
    INVALID = 4
};

struct ParameterAndDescription : public TreeItem
{
    ParameterAndDescription() : m_parameter(""), m_type(""),
        m_description(""), m_direction(""),
        m_source(INVALID), m_progLangType("")
    {}

    ParameterAndDescription(const QString& name, const QString& type,
                            const QString& descr, const QString& dir,
                            PARAMETER_SOURCE source) :
        m_parameter(name), m_type(type), m_description(descr), m_direction(dir),
        m_source(source)
    {}

    ParameterAndDescription(const QString& name, const QString& type) :
        m_parameter(name), m_type(type), m_description(""), m_direction(""),
        m_source(DOC)
    {}

    QString m_parameter;
    QString m_type;
    QString m_description;
    QString m_direction;
    PARAMETER_SOURCE m_source; // 0 - doc, 1 - parameter list, 2 - verified in both

    // to what type it's mapped in the programming language (Java for ex)
    QString m_progLangType;
};

#endif // CORE_PARAMETERANDDESCRIPTION_H
