#include <QtGui>

#include "MajorVersion.h"
#include "UserDataType.h"
#include "Table.h"

MajorVersion::MajorVersion(QTreeWidget* tree, QTreeWidgetItem* projectItem, int ver) : m_tree(tree), m_projectItem(projectItem)
{
    // make the dts sub item coming from the project
    QString v = QString::number(ver);
    version = v + ".0";
    v = QString("Ver: ") + v;
    v = v + ".0";

    createTreeItems();
}

MajorVersion::MajorVersion(QString verAsString):version(verAsString)
{
}

void MajorVersion::createTreeItems()
{
    QIcon tablesIcon(":/images/actions/images/small/table.png");
    QIcon viewsIcon(":/images/actions/images/small/view_icon.png");
    QIcon dtsIcon(":/images/actions/images/small/datatypes.PNG");
    QIcon versionIcon(":/images/actions/images/small/version.PNG");

    versionItem = new QTreeWidgetItem(m_projectItem, QStringList(QString("Ver: ") + version)) ;
    versionItem->setIcon(0, versionIcon);
    m_tree->addTopLevelItem(versionItem);

    // make the dts sub item coming from the project
    dtsItem = new QTreeWidgetItem(versionItem, QStringList(QObject::tr("Data Types"))) ;
    dtsItem->setIcon(0, dtsIcon);
    m_tree->addTopLevelItem(dtsItem);

    // make the tables sub item coming from the project
    tablesItem = new QTreeWidgetItem(versionItem, QStringList(QObject::tr("Tables"))) ;
    tablesItem->setIcon(0, tablesIcon);
    m_tree->addTopLevelItem(tablesItem);

    // make the views sub item coming from the project
    viewsItem = new QTreeWidgetItem(versionItem, QStringList(QObject::tr("Views"))) ;
    viewsItem->setIcon(0, viewsIcon);
    m_tree->addTopLevelItem(viewsItem);
}

void MajorVersion::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement majorVersionElement = doc.createElement("MajorVersion");      // will hold the data in this element

    QDomElement versionElement = doc.createElement("Version");        // the version node
    QDomText versionNode = doc.createTextNode(version);
    versionElement.appendChild(versionNode);
    majorVersionElement.appendChild(versionElement);

    // now saving the data types
    {
    QDomElement dataTypesElement = doc.createElement("DataTypes");
    for(int i=0; i< m_dataTypes.size(); i++)
    {
        m_dataTypes[i]->serialize(doc, dataTypesElement);
    }
    majorVersionElement.appendChild(dataTypesElement);
    }

    {
    QDomElement tablesElement = doc.createElement("Tables");
    for(int i=0; i< m_tables.size(); i++)
    {
        m_tables[i]->serialize(doc, tablesElement);
    }
    majorVersionElement.appendChild(tablesElement);
    }

    parent.appendChild(majorVersionElement);

}

inline void MajorVersion::addNewDataType(UserDataType* dt)
{
    m_dataTypes.append(dt);
}

bool MajorVersion::hasDataType(const QString& name) const
{
    for(int i=0; i < m_dataTypes.size(); i++)
    {
        if(m_dataTypes.at(i)->getName() == name)
        {
            return true;
        }
    }
    return false;
}

UserDataType* MajorVersion::getDataType(const QString& name)
{
    for(int i=0; i< m_dataTypes.size(); i++)
    {
        if(m_dataTypes.at(i)->getName() == name)
        {
            return const_cast<UserDataType*>(m_dataTypes.at(i));  // yeah, this sucks
        }
    }
    return 0;
}

int MajorVersion::getDataTypeIndex(const QString& name)
{
    for(int i=0; i < m_dataTypes.size(); i++)
    {
        if(m_dataTypes.at(i)->getName() == name)
        {
            return i;
        }
    }
    return -1;
}

inline void MajorVersion::addTable(Table *t)
{
    m_tables.append(t);
}


inline bool MajorVersion::hasTable(Table *t)
{
    return m_tables.indexOf(t) >= 0;
}


Table* MajorVersion::getTable(const QString &name)
{
    for(int i=0; i< m_tables.size(); i++)
    {
        if(m_tables[i]->getName() == name)
        {
            return m_tables[i];
        }
    }
    return 0;
}

inline const QVector<UserDataType*>& MajorVersion::getDataTypes() const
{
    return m_dataTypes;
}

inline const QVector<Table*>& MajorVersion::getTables() const
{
    return m_tables;
}
