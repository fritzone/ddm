#include "DeserializationFactory.h"
#include "UserDataType.h"
#include "Index.h"
#include "Table.h"
#include "Column.h"
#include "MajorVersion.h"
#include "Solution.h"
#include "DatabaseEngine.h"
#include "ForeignKey.h"

#include <QStringList>

DeserializationFactory::DeserializationFactory()
{
}

UserDataType* DeserializationFactory::createUserDataType(const QDomDocument& doc, const QDomElement& element)
{
    QString name = element.attribute("Name");
    QString type = element.attribute("Type");
    QString sqlType = element.attribute("SqlType");
    QString size = element.attribute("Size");
    QString defaultValue = element.attribute("DefaultValue");
    QString codepage = element.attribute("Codepage");
    QString isUnsigned = element.attribute("Unsigned");
    QString canBeNull = element.attribute("CanBeNull");
    QString autoInc = element.attribute("CanBeNull");
    QString description = "";
    QStringList miscValues;

    for(int i=0; i<element.childNodes().size(); i++)
    {
        if(element.childNodes().at(i).nodeName() == "Description")
        {
            QDomNode descNode = element.childNodes().at(i);
            description = descNode.firstChild().nodeValue();
        }

        if(element.childNodes().at(i).nodeName() == "Values")
        {
            QDomNode valuesNode = element.childNodes().at(i);
            for(int j=0; j<valuesNode.childNodes().count(); j++)
            {
                miscValues.append(valuesNode.childNodes().at(j).firstChild().nodeValue());
            }
        }
    }

    UserDataType* result = new UserDataType(name, type, sqlType, size, defaultValue, codepage, miscValues, isUnsigned=="1", description, canBeNull == "1", autoInc == "1");

    return result;

}

ForeignKey* DeserializationFactory::createForeignKey(Table *tab, const QDomDocument &doc, const QDomElement &element)
{
    QString name = element.attribute("Name");
    QString onUpdate = element.attribute("OnUpdate");
    QString onDelete = element.attribute("OnDelete");

    ForeignKey* fk = new ForeignKey();
    fk->setName(name);

    QDomNodeList associationsNodeList = element.firstChild().childNodes();
    for(int i=0; i<associationsNodeList.size(); i++)
    {
        QDomElement assocElement = associationsNodeList.at(i).toElement();
        QString foreignTable = assocElement.attribute("ForeignTable");
        QString localTable = assocElement.attribute("LocalTable");
        QString foreignColumn = assocElement.attribute("ForeignColumn");
        QString localColumn = assocElement.attribute("LocalColumn");

        ForeignKey::ColumnAssociation* fkas = new ForeignKey::ColumnAssociation(foreignTable, foreignColumn, localTable, localColumn);
        fk->addAssociation(fkas);
    }
    return fk;

}

Index* DeserializationFactory::createIndex(Table* table, const QDomDocument &doc, const QDomElement &element)
{
    QString name = element.attribute("Name");
    QString type = element.attribute("Type");

    Index* result = new Index(name, type);

    for(int i=0; i<element.childNodes().size(); i++)
    {
        if(element.childNodes().at(i).nodeName() == "Columns")
        {
            for(int j=0; j<element.childNodes().at(i).childNodes().size(); j++) // iterating through the "Column" nodes
            {
                result->addColumn(table->getColumn(element.childNodes().at(i).childNodes().at(j).toElement().attribute("Name")));    // finding the Name attribute
            }
        }
    }

    return result;
}

MajorVersion* DeserializationFactory::createMajorVersion(const QDomDocument &doc, const QDomElement &element)
{
    QString name = "";

    for(int i=0; i<element.childNodes().count(); i++)
    {
        if(element.childNodes().at(i).nodeName() == "Version")
        {
            name = element.childNodes().at(i).firstChild().nodeValue();// first child is a text node
        }
    }

    MajorVersion* result = new MajorVersion(name);

    // getting the data types
    for(int i=0; i<element.childNodes().count(); i++)
    {
        if(element.childNodes().at(i).nodeName() == "DataTypes")
        {
            for(int j=0; j<element.childNodes().at(i).childNodes().count(); j++)
            {
                UserDataType* dt = createUserDataType(doc, element.childNodes().at(i).childNodes().at(j).toElement());
                result->addNewDataType(dt);
            }
        }
    }

    // getting the tables
    for(int i=0; i<element.childNodes().count(); i++)
    {
        if(element.childNodes().at(i).nodeName() == "Tables")   // in a well formatted result, the Tables child node is always after the datatypes
        {
            for(int j=0; j<element.childNodes().at(i).childNodes().count(); j++)
            {
                Table* tab = createTable(result, doc, element.childNodes().at(i).childNodes().at(j).toElement());
                result->addTable(tab);
            }
        }
    }

    // now update the tables so that the foreign keys get populated
    for(int i=0; i<result->getTables().size(); i++)
    {
        Table* tabI = result->getTables().at(i);
        for(int j=0; j<tabI->getFks().size(); j++)
        {
            ForeignKey* fkJ = tabI->getForeignKey(j);
            for(int k=0; k<fkJ->getAssociations().size(); k++)
            {
                ForeignKey::ColumnAssociation* assK = fkJ->getAssociation(k);
                // first: set the tables
                for(int l=0; l<result->getTables().size(); l++)
                {
                    if(result->getTables().at(l)->getName() == assK->getSForeignTable())
                    {
                        assK->setForeignTable(result->getTables().at(l));
                    }
                    if(result->getTables().at(l)->getName() == assK->getSLocalTable())
                    {
                        assK->setLocalTable(result->getTables().at(l));
                    }
                }
                // then: set the columns of those tables
                for(int l=0; l<assK->getLocalTable()->columns().size(); l++)
                {
                    Column* colL = assK->getLocalTable()->getColumns().at(l);
                    if(colL->getName() == assK->getSLocalColumn())
                    {
                        assK->setLocalColumn(colL);
                    }
                }
                for(int l=0; l<assK->getForeignTable()->columns().size(); l++)
                {
                    Column* colL = assK->getForeignTable()->getColumns().at(l);
                    if(colL->getName() == assK->getSForeignColumn())
                    {
                        assK->setForeignColumn(colL);
                    }
                }
            }
        }
    }

    return result;
}

Column* DeserializationFactory::createColumn(MajorVersion* ver, const QDomDocument &doc, const QDomElement &element)
{
    QString name = element.attribute("Name");
    QString pk = element.attribute("PK");
    QString type = element.attribute("Type");
    UserDataType* udt = ver->getDataType(type);

    Column* col = new Column(name, udt, pk == "1");
    return col;
}

Table* DeserializationFactory::createTable(MajorVersion* ver, const QDomDocument &doc, const QDomElement &element)
{
    Table* result = new Table();
    QString name = element.attribute("Name");
    result->setName(name);
    result->setPersistent(element.attribute("Persistent")=="1");

    for(int i=0; i<element.childNodes().count(); i++)
    {
        if(element.childNodes().at(i).nodeName() == "Description")
        {
            result->setDescription(element.childNodes().at(i).firstChild().nodeValue());
        }
        if(element.childNodes().at(i).nodeName() == "Columns")
        {
            for(int j=0; j<element.childNodes().at(i).childNodes().count(); j++)
            {
                Column* col = createColumn(ver, doc, element.childNodes().at(i).childNodes().at(j).toElement());
                result->addColumn(col);
            }
        }
        if(element.childNodes().at(i).nodeName() == "Indices")
        {
            for(int j=0; j<element.childNodes().at(i).childNodes().count(); j++)
            {
                Index* idx = createIndex(result, doc, element.childNodes().at(i).childNodes().at(j).toElement());
                result->addIndex(idx);
            }
        }
        if(element.childNodes().at(i).nodeName() == "ForeignKeys")
        {
            for(int j=0; j<element.childNodes().at(i).childNodes().count(); j++)
            {
                ForeignKey* fk = createForeignKey(result, doc, element.childNodes().at(i).childNodes().at(j).toElement());
                result->addForeignKey(fk);
            }
        }
        if(element.childNodes().at(i).nodeName() == "StartupValues")
        {
            QVector <QVector <QString> > values;
            for(int j=0; j<element.childNodes().at(i).childNodes().count(); j++)    // willl count the rows
            {
                QDomElement childJ = element.childNodes().at(i).childNodes().at(j).toElement();
                QVector <QString> rowJ;
                for(int k=0; k<element.childNodes().at(i).childNodes().at(j).childNodes().count(); k++) // Datas in the row
                {
                    rowJ.append(element.childNodes().at(i).childNodes().at(j).childNodes().at(k).toElement().attribute("Value"));
                }
                values.append(rowJ);
            }
            result->setDefaultValues(values);
        }
    }

    return result;
}

Project* DeserializationFactory::createProject(const QDomDocument &doc, const QDomElement &element)
{
    Project* prj = new Project(element.attribute("Name"));
    prj->setEngine(DatabaseEngine::createEngine(element.attribute("DB")));

    QDomNodeList majorVersionNodes = element.firstChild().childNodes();
    for(int i=0; i<majorVersionNodes.count(); i++)
    {
        MajorVersion* majVer = createMajorVersion(doc, majorVersionNodes.at(i).toElement());
        prj->addMajorVersion(majVer);
    }

    return prj;

}

Solution* DeserializationFactory::createSolution(const QDomDocument &doc, const QDomElement &element)
{
    Solution* sol = new Solution(element.attribute("Name"));

    QDomNodeList projectElements = element.firstChild().childNodes();
    for(int i=0; i<projectElements.count(); i++) //
    {
        Project* prj = createProject(doc, projectElements.at(i).toElement());
        sol->addProject(prj);
    }

    return sol;
}
