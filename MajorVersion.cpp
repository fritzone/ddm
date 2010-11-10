#include "MajorVersion.h"
#include "UserDataType.h"
#include "Table.h"
#include "Diagram.h"
#include "TableInstance.h"

MajorVersion::MajorVersion(QTreeWidget* tree, QTreeWidget* dttree, ContextMenuEnabledTreeWidgetItem* projectItem, int ver, Project* p)
    : DefaultVersionImplementation(tree, dttree, projectItem, p)
{
    // make the dts sub item coming from the project
    QString v = QString::number(ver);
    version = v + ".0";
    v = QString("Ver: ") + v;
    v = v + ".0";

    createTreeItems();
}

MajorVersion::MajorVersion(QString verAsString, Project* p) : DefaultVersionImplementation(0,0,0,p)
{
    version = verAsString;
}

void MajorVersion::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement majorVersionElement = doc.createElement("MajorVersion");      // will hold the data in this element

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

    parent.appendChild(majorVersionElement);

}
