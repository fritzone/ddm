#include "Configuration.h"
#include "strings.h"

#include <QSettings>

Configuration::Configuration() : m_useDefaultLengths(true), m_allowForeignKeyPropagation(false), m_drawTableTypes(false), m_continuousValidation(false), m_sqlOpts()
{
    readFromFile();
}

Configuration::~Configuration()
{
    writeToFile();
}


void Configuration::writeToFile()
{
    QSettings s(strUnauthorizedFrog, strDDM);

    s.setValue(strDrawTableTypes, m_drawTableTypes);
    s.setValue(strAllowFkPropagation, m_allowForeignKeyPropagation);
    s.setValue(strUseDefaultLengths, m_useDefaultLengths);
    s.setValue(strContinuousValidation, m_continuousValidation);
    s.setValue(strCase, m_sqlOpts[strCase]);
    s.setValue(strBackticks, m_sqlOpts[strBackticks]);
    s.setValue(strGenerateComments, m_sqlOpts[strGenerateComments]);
    s.setValue(strPKSposition, m_sqlOpts[strPKSposition]);
    s.setValue(strFKSposition, m_sqlOpts[strFKSposition]);
    s.setValue(strNewLineBetweenSelectExpressionsInSqlGeneration, m_sqlOpts[strNewLineBetweenSelectExpressionsInSqlGeneration]);

    s.sync();
}

void Configuration::readFromFile()
{
    QSettings s(strUnauthorizedFrog, strDDM);

    m_drawTableTypes = s.value(strDrawTableTypes, QVariant(false)).toBool();
    m_allowForeignKeyPropagation = s.value(strAllowFkPropagation, QVariant(false)).toBool();
    m_useDefaultLengths = s.value(strUseDefaultLengths, QVariant(true)).toBool();
    m_continuousValidation = s.value(strContinuousValidation, QVariant(false)).toBool();
    m_sqlOpts[strCase] = s.value(strCase, QVariant(strLower)).toString();
    m_sqlOpts[strBackticks] = s.value(strBackticks, QVariant(strNo)).toString();
    m_sqlOpts[strGenerateComments] = s.value(strGenerateComments, QVariant(strYes)).toString();
    m_sqlOpts[strPKSposition] = s.value(strPKSposition, QVariant(strColumnDeclaration)).toString();
    m_sqlOpts[strFKSposition] = s.value(strFKSposition, QVariant(strInTable)).toString();
    m_sqlOpts[strNewLineBetweenSelectExpressionsInSqlGeneration] = s.value(strNewLineBetweenSelectExpressionsInSqlGeneration, QVariant(strNo)).toString();

}
