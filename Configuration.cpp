#include "Configuration.h"
#include "strings.h"

#include <QDomDocument>
#include <QFile>

Configuration::Configuration() : useDefaultLengths(true), m_allowForeignKeyPropagation(false), m_drawTableTypes(false), m_sqlOpts()
{
    m_sqlOpts["Case"] = "Lower";
    m_sqlOpts["Backticks"] = "No";
    m_sqlOpts["GenerateComments"] = "Yes";
    m_sqlOpts["PKSposition"] = "ColumnDeclaration";
    m_sqlOpts["FKSposition"] = "InTable";
    m_sqlOpts[strNewLineBetweenSelectExpressionsInSqlGeneration] = "No";

    readFromFile();
}

Configuration::~Configuration()
{
    writeToFile();
}


void Configuration::writeToFile()
{
    QDomDocument doc("DDM-CONFIG");
    QDomElement root = doc.createElement("Configuration");

    QDomElement diagrams = doc.createElement("Diagrams");
    diagrams.setAttribute("DrawTableTypes", m_drawTableTypes);
    root.appendChild(diagrams);

    QDomElement behavior = doc.createElement("Behavior");
    behavior.setAttribute("AllowFkPropagation", m_allowForeignKeyPropagation);
    behavior.setAttribute("UseDefaultLengths", useDefaultLengths);
    behavior.setAttribute("ContinuousValidation", m_continuousValidation);
    root.appendChild(behavior);

    QDomElement sqlGeneration = doc.createElement("SqlGeneration");
    sqlGeneration.setAttribute("Case", m_sqlOpts["Case"]);
    sqlGeneration.setAttribute("Backticks", m_sqlOpts["Backticks"]);
    sqlGeneration.setAttribute("GenerateComments", m_sqlOpts["GenerateComments"]);
    sqlGeneration.setAttribute("PKSposition", m_sqlOpts["PKSposition"]);
    sqlGeneration.setAttribute("FKSposition", m_sqlOpts["FKSposition"]);
    sqlGeneration.setAttribute(strNewLineBetweenSelectExpressionsInSqlGeneration, m_sqlOpts[strNewLineBetweenSelectExpressionsInSqlGeneration]);
    root.appendChild(sqlGeneration);

    doc.appendChild(root);

    QString xml = doc.toString();
    QFile f1("ddm.cfx");
    if (!f1.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }
    f1.write(xml.toAscii());
    f1.close();
}

void Configuration::readFromFile()
{
    QDomDocument doc ("DDM-CONFIG");
    QFile file ("ddm.cfx");
    if (!file.open(QIODevice::ReadOnly)) return;
    if (!doc.setContent(&file))
    {
        file.close();
        return;
    }
    file.close();
    QDomElement docElem = doc.documentElement();
    for(int i=0; i<docElem.childNodes().size(); i++)
    {
        if(docElem.childNodes().at(i).nodeName() == "Diagrams")
        {
            m_drawTableTypes = docElem.childNodes().at(i).toElement().attribute("DrawTableTypes") == "1";
        }
        if(docElem.childNodes().at(i).nodeName() == "Behavior")
        {
            useDefaultLengths = docElem.childNodes().at(i).toElement().attribute("UseDefaultLengths") == "1";
            m_allowForeignKeyPropagation = docElem.childNodes().at(i).toElement().attribute("AllowFkPropagation") == "1";
            m_continuousValidation = docElem.childNodes().at(i).toElement().attribute("ContinuousValidation") == "1";
        }
        if(docElem.childNodes().at(i).nodeName() == "SqlGeneration")
        {
            for(int j=0; j<docElem.childNodes().at(i).attributes().size(); j++)
            {
                m_sqlOpts[docElem.childNodes().at(i).attributes().item(j).nodeName()] =
                        docElem.childNodes().at(i).toElement().attribute(docElem.childNodes().at(i).attributes().item(j).nodeName());
            }
        }
    }

}

bool Configuration::operator [](const QString&)
{
    return false;
}
