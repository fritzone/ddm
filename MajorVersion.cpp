#include "MajorVersion.h"
#include "UserDataType.h"
#include "Table.h"
#include "Diagram.h"
#include "TableInstance.h"
#include "core_View.h"
#include "core_Procedure.h"
#include "core_Trigger.h"
#include "core_Function.h"

MajorVersion::MajorVersion(int major, int minor, Project* p) : DefaultVersionImplementation(p, major, minor)
{
    // make the dts sub item coming from the project
    QString v = QString::number(major);
    QString v1 = QString::number(minor);
    version = v + "." + v1;
}

MajorVersion::MajorVersion(QString verAsString, Project* p) : DefaultVersionImplementation(p, 0, 0)
{
    version = verAsString;
}

void MajorVersion::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement majorVersionElement = doc.createElement("MajorVersion");      // will hold the data in this element

    majorVersionElement.setAttribute("major", m_major);
    majorVersionElement.setAttribute("minor", m_minor);

    QDomElement versionElement = doc.createElement("Version");        // the version node
    QDomText versionNode = doc.createTextNode(version);
    versionElement.appendChild(versionNode);
    majorVersionElement.appendChild(versionElement);

    // the data types
    {
    QDomElement dataTypesElement = doc.createElement("DataTypes");
    for(int i=0; i< m_data.m_dataTypes.size(); i++)
    {
        m_data.m_dataTypes[i]->serialize(doc, dataTypesElement);
    }
    majorVersionElement.appendChild(dataTypesElement);
    }

    // the tables
    {
    QDomElement tablesElement = doc.createElement("Tables");
    for(int i=0; i< m_data.m_tables.size(); i++)
    {
        m_data.m_tables[i]->serialize(doc, tablesElement);
    }
    majorVersionElement.appendChild(tablesElement);
    }

    // the diagrams
    {
    QDomElement diagramsElement = doc.createElement("Diagrams");
    for(int i=0; i< m_data.m_diagrams.size(); i++)
    {
        m_data.m_diagrams[i]->serialize(doc, diagramsElement);
    }
    majorVersionElement.appendChild(diagramsElement);
    }

    // the table instances
    {
    QDomElement tableInstancesElement = doc.createElement("TableInstances");
    for(int i=0; i< m_data.m_tableInstances.size(); i++)
    {
        m_data.m_tableInstances[i]->serialize(doc, tableInstancesElement);
    }
    majorVersionElement.appendChild(tableInstancesElement);
    }

    // the views
    {
    QDomElement viewsElement = doc.createElement("Views");
    for(int i=0; i< m_data.m_views.size(); i++)
    {
        m_data.m_views[i]->serialize(doc, viewsElement);
    }
    majorVersionElement.appendChild(viewsElement);
    }

    // the procedures
    {
    QDomElement procElement = doc.createElement("Procedures");
    for(int i=0; i< m_data.m_procedures.size(); i++)
    {
        m_data.m_procedures[i]->serialize(doc, procElement);
    }
    majorVersionElement.appendChild(procElement);
    }

    // the functions
    {
    QDomElement procElement = doc.createElement("Functions");
    for(int i=0; i< m_data.m_procedures.size(); i++)
    {
        m_data.m_functions[i]->serialize(doc, procElement);
    }
    majorVersionElement.appendChild(procElement);
    }
    // the triggers
    {
    QDomElement triggerElement = doc.createElement("Triggers");
    for(int i=0; i< m_data.m_triggers.size(); i++)
    {
        m_data.m_triggers[i]->serialize(doc, triggerElement);
    }
    majorVersionElement.appendChild(triggerElement);
    }

    parent.appendChild(majorVersionElement);

}

