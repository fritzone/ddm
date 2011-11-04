#include "DeserializationFactory.h"
#include "UserDataType.h"
#include "Index.h"
#include "Table.h"
#include "Column.h"
#include "MajorVersion.h"
#include "Solution.h"
#include "db_DatabaseEngine.h"
#include "ForeignKey.h"
#include "db_AbstractStorageEngineListProvider.h"
#include "db_AbstractStorageEngine.h"
#include "DiagramTableDescriptor.h"
#include "DiagramNoteDescriptor.h"
#include "DiagramFKDescriptor.h"
#include "Diagram.h"
#include "TableInstance.h"
#include "core_View.h"

#include <QStringList>

DeserializationFactory::DeserializationFactory()
{
}

UserDataType* DeserializationFactory::createUserDataType(const QDomDocument&, const QDomElement& element)
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

ForeignKey* DeserializationFactory::createForeignKey(Table *, const QDomDocument &, const QDomElement &element)
{
    QString name = element.attribute("Name");
    QString onUpdate = element.attribute("OnUpdate");
    QString onDelete = element.attribute("OnDelete");

    ForeignKey* fk = new ForeignKey();
    fk->setName(name);
    fk->setOnDelete(onDelete);
    fk->setOnUpdate(onUpdate);

    for(int i=0; i<element.childNodes().count(); i++)
    {
        if(element.childNodes().at(i).nodeName() == "Associations")
        {
            QDomNodeList associationsNodeList = element.childNodes().at(i).childNodes();
            for(int j=0; j<associationsNodeList.size(); j++)
            {
                QDomElement assocElement = associationsNodeList.at(j).toElement();
                QString foreignTable = assocElement.attribute("ForeignTable");
                QString localTable = assocElement.attribute("LocalTable");
                QString foreignColumn = assocElement.attribute("ForeignColumn");
                QString localColumn = assocElement.attribute("LocalColumn");

                ForeignKey::ColumnAssociation* fkas = new ForeignKey::ColumnAssociation(foreignTable, foreignColumn, localTable, localColumn);
                fk->addAssociation(fkas);
            }
        }
        if(element.childNodes().at(i).nodeName() == "AutoGenIndices")
        {
            QDomNodeList autogeIndicesNodeList = element.childNodes().at(i).childNodes();
            for(int j=0; j<autogeIndicesNodeList.size(); j++)
            {
                QDomElement autogenIndicesElement = autogeIndicesNodeList.at(j).toElement();
                QString tabName = autogenIndicesElement.attribute("Table");
                QString indexName = autogenIndicesElement.attribute("Name");
                ForeignKey::TableIndexTempStructure a;
                a.indexName = indexName; a.tabName = tabName;
                fk->addTempAutogenIndex(a);
            }
        }
    }
    return fk;

}

Index* DeserializationFactory::createIndex(Table* table, const QDomDocument &, const QDomElement &element)
{
    QString name = element.attribute("Name");
    QString type = element.attribute("Type");

    Index* result = new Index(name, type, table);

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

MajorVersion* DeserializationFactory::createMajorVersion(Project* p, DatabaseEngine* engine, const QDomDocument &doc, const QDomElement &element)
{
    QString name = "";

    for(int i=0; i<element.childNodes().count(); i++)
    {
        if(element.childNodes().at(i).nodeName() == "Version")
        {
            name = element.childNodes().at(i).firstChild().nodeValue();// first child is a text node
        }
    }

    MajorVersion* result = new MajorVersion(name, p);

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
                Table* tab = createTable(engine, result, doc, element.childNodes().at(i).childNodes().at(j).toElement());
                result->addTable(tab);
            }
        }
    }

    // update the parent child relationships of the table
    result->setupTableParentChildRelationships();

    // now update the tables so that the foreign keys get populated
    for(int i=0; i<result->getTables().size(); i++)
    {
        Table* tabI = result->getTables().at(i);
        result->setupForeignKeyRelationshipsForATable(tabI);
    }

    // now update the foreign keys of the tables so that the auto generated indices get populated properly
    for(int i=0; i<result->getTables().size(); i++)
    {
        Table* tabI = result->getTables().at(i);
        for(int j=0; j<tabI->getFks().size(); j++)
        {
            ForeignKey* fkJ = tabI->getFks()[j];
            fkJ->finalizeAutogeneratedIndices(result);
        }
    }

    // getting the diagrams
    for(int i=0; i<element.childNodes().count(); i++)
    {
        if(element.childNodes().at(i).nodeName() == "Diagrams")   // in a well formatted result, the Diagrams child node is always after the tables
        {
            for(int j=0; j<element.childNodes().at(i).childNodes().count(); j++)
            {
                Diagram* diagram = createDiagram(result, doc, element.childNodes().at(i).childNodes().at(j).toElement());
                result->addDiagram(diagram);
            }
        }
    }

    // getting the table instances
    for(int i=0; i<element.childNodes().count(); i++)
    {
        if(element.childNodes().at(i).nodeName() == "TableInstances")
        {
            for(int j=0; j<element.childNodes().at(i).childNodes().count(); j++)
            {
                TableInstance* tabInst = createTableInstance(result, doc, element.childNodes().at(i).childNodes().at(j).toElement());
                result->addTableInstance(tabInst);
            }

            // and here populate the "instantiated" like stuff
            for(int j=0; j<element.childNodes().at(i).childNodes().count(); j++)
            {
                createTableInstance(result, doc, element.childNodes().at(i).childNodes().at(j).toElement(), true);
            }
        }
    }

    // getting the views
    for(int i=0; i<element.childNodes().count(); i++)
    {
        if(element.childNodes().at(i).nodeName() == "Views")
        {
            for(int j=0; j<element.childNodes().at(i).childNodes().count(); j++)
            {
                View* view = createView(result, doc, element.childNodes().at(i).childNodes().at(j).toElement());
                result->addView(view);
            }
        }
    }

    return result;
}

View* DeserializationFactory::createView(Version* v, const QDomDocument& doc, const QDomElement& element)
{
    bool manual = element.attribute("Manual") == "1";
    View* view = new View(manual);
    QString name = element.attribute("Name");
    view->setName(name);
    if(manual)
    {
        //QDomElement e =
    }

    return view;
}

Column* DeserializationFactory::createColumn(Version* ver, const QDomDocument &, const QDomElement &element)
{
    QString name = element.attribute("Name");
    QString pk = element.attribute("PK");
    QString type = element.attribute("Type");
    UserDataType* udt = ver->getDataType(type);

    Column* col = new Column(name, udt, pk == "1", element.attribute("AutoIncrement")=="1");
    return col;
}

Table* DeserializationFactory::createTable(DatabaseEngine* engine, Version* ver, const QDomDocument &doc, const QDomElement &element)
{
    Table* result = new Table(ver);
    QString name = element.attribute("Name");
    result->setName(name);
    result->setPersistent(element.attribute("Persistent")=="1");
    result->setTemporary(element.attribute("Temporary")=="1");
    result->setTempTabName(element.attribute("Parent"));

    QString stEngineName = element.attribute("StorageEngine");
    AbstractStorageEngineListProvider* lp = engine->getStorageEngineListProviders();
    QVector<AbstractStorageEngine*>const & engines = lp->getStorageEngines();
    AbstractStorageEngine* ceng = 0;
    for(int i=0; i<engines.size(); i++)
    {
        if(engines.at(i)->name() == stEngineName)
        {
            ceng = engines.at(i);
        }
    }

    result->setStorageEngine(ceng);

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
    Project* prj = new Project(element.attribute("Name"), element.attribute("OOP")=="1");
    DatabaseEngine* engine = DatabaseEngine::createEngine(element.attribute("DB"));
    prj->setEngine(engine);

    for(int i=0; i<element.childNodes().count(); i++)
    {
        if(element.childNodes().at(i).nodeName() == "MajorVersions")
        {
            QDomNodeList majorVersionNodes = element.childNodes().at(i).childNodes();
            for(int i=0; i<majorVersionNodes.count(); i++)
            {
                MajorVersion* majVer = createMajorVersion(prj, engine, doc, majorVersionNodes.at(i).toElement());
                prj->addMajorVersion(majVer);
            }
        }
        else
        if(element.childNodes().at(i).nodeName() == "Description")
        {
            prj->setDescription(element.childNodes().at(i).firstChild().nodeValue());
        }
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

DiagramTableDescriptor* DeserializationFactory::createDiagramTableDescriptor(const QDomDocument &, const QDomElement &element)
{
    QString tabName = element.attribute("Name");
    int x = element.attribute("x").toInt();
    int y = element.attribute("y").toInt();

    DiagramTableDescriptor* result = new DiagramTableDescriptor(tabName, x, y);
    return result;
}

DiagramNoteDescriptor* DeserializationFactory::createDiagramNoteDescriptor(const QDomDocument &, const QDomElement &element)
{
    int x = element.attribute("x").toInt();
    int y = element.attribute("y").toInt();
    bool framed = element.attribute("frame")=="1";
    QDomElement textNode = element.firstChild().toElement();
    QDomNode cdataNode = textNode.firstChild();
    QString text = cdataNode.nodeValue();

    DiagramNoteDescriptor* result = new DiagramNoteDescriptor(text, x, y, framed);
    return result;
}

DiagramFKDescriptor* DeserializationFactory::createDiagramFKDescriptor(const QDomDocument&, const QDomElement& element)
{
    QString tab1 = element.attribute("tab1");
    QString tab2 = element.attribute("tab2");
    int x = element.attribute("x").toInt();
    int y = element.attribute("y").toInt();
    qreal ellx = element.attribute("ellipseX").toFloat();
    qreal elly = element.attribute("ellipseY").toFloat();

    qreal l1otx = element.attribute("line1OthX").toFloat();
    qreal l1oty = element.attribute("line1OthY").toFloat();

    qreal l1posx = element.attribute("line1PosX").toFloat();
    qreal l1posy = element.attribute("line1PosY").toFloat();

    qreal l2otx = element.attribute("line2OthX").toFloat();
    qreal l2oty = element.attribute("line2OthY").toFloat();

    qreal l2posx = element.attribute("line2PosX").toFloat();
    qreal l2posy = element.attribute("line2PosY").toFloat();

    qreal rel1posx = element.attribute("rel1PosX").toFloat();
    qreal rel1posy = element.attribute("rel1PosY").toFloat();

    qreal rel2posx = element.attribute("rel2PosX").toFloat();
    qreal rel2posy = element.attribute("rel2PosY").toFloat();

    qreal arrowp1x = element.attribute("arrowP1x").toFloat();
    qreal arrowp1y = element.attribute("arrowP1y").toFloat();

    qreal arrowp2x = element.attribute("arrowP2x").toFloat();
    qreal arrowp2y = element.attribute("arrowP2y").toFloat();

    qreal arrowposx = element.attribute("arrowPosx").toFloat();
    qreal arrowposy = element.attribute("arrowPosy").toFloat();

    QString fkn = element.attribute("fkn");

    DiagramFKDescriptor* dfk = new     DiagramFKDescriptor(tab1, tab2, x, y,  ellx,  elly,  l1otx,  l1oty,  l1posx, l1posy,  l2otx,  l2oty,  l2posx,
                                                           l2posy,  rel1posx,  rel1posy,  rel2posx,  rel2posy,
                                                           arrowp1x,  arrowp1y,  arrowp2x,  arrowp2y,  arrowposx,  arrowposy, fkn);

    return dfk;

}

Diagram* DeserializationFactory::createDiagram(Version* v, const QDomDocument &doc, const QDomElement &element)
{
    QString name = element.attribute("Name");
    Diagram* result = new Diagram(v, name);

    for(int i=0; i<element.childNodes().count(); i++)
    {
        if(element.childNodes().at(i).nodeName() == "Tables")
        {
            for(int j=0; j<element.childNodes().at(i).childNodes().count(); j++)
            {
                DiagramTableDescriptor* tabDesc = createDiagramTableDescriptor(doc, element.childNodes().at(i).childNodes().at(j).toElement());
                result->addTableDescriptor(tabDesc);
            }
        }

        if(element.childNodes().at(i).nodeName() == "Notes")
        {
            for(int j=0; j<element.childNodes().at(i).childNodes().count(); j++)
            {
                DiagramNoteDescriptor* noteDesc = createDiagramNoteDescriptor(doc, element.childNodes().at(i).childNodes().at(j).toElement());
                result->addNoteDescriptor(noteDesc);
            }
        }

        if(element.childNodes().at(i).nodeName() == "FKs")
        {
            for(int j=0; j<element.childNodes().at(i).childNodes().count(); j++)
            {
                DiagramFKDescriptor* fkDesc = createDiagramFKDescriptor(doc, element.childNodes().at(i).childNodes().at(j).toElement());
                result->addFKDescriptor(fkDesc);
            }
        }
    }

    // means, there's a tree item for this diagram
    result->setSaved(true);
    return result;
}

TableInstance* DeserializationFactory::createTableInstance(Version* v, const QDomDocument&, const QDomElement &element, bool secondStep)
{
    QString name = element.attribute("Name");
    QString tabName = element.attribute("Table");
    bool refed = element.attribute("Ref") == "1";
    QString refTables = element.attribute("ReferencingTables");
    QString instantiatedTableInstances = element.attribute("InstantiatedTableInstances");

    QHash <QString, QVector<QString> > data;
    for(int i=0; i<element.childNodes().count(); i++)
    {
        QString colName = element.childNodes().at(i).toElement().attribute("Name");
        QVector<QString> col;
        for(int j=0; j<element.childNodes().at(i).toElement().childNodes().count(); j++)
        {
            col.append(element.childNodes().at(i).toElement().childNodes().at(j).toElement().attribute("value"));
        }
        data[colName] = col;
    }

    if(secondStep)
    {
        TableInstance* result = v->getTableInstance(name);
        QStringList lst2 = instantiatedTableInstances.split(QChar(','), QString::SkipEmptyParts);
        for(int i=0; i<lst2.size(); i++)
        {
            TableInstance* tabInst= v->getTableInstance(lst2.at(i));
            result->addInstantiatedTableInstance(tabInst);
        }
        return 0;
    }
    else
    {
        TableInstance* result = new TableInstance(v->getTable(tabName), refed);

        QStringList lst = refTables.split(QChar(','), QString::SkipEmptyParts);
        for(int i=0; i<lst.size(); i++)
        {
            Table* tab = v->getTable(lst.at(i));
            result->addTableReferencingThis(tab);
        }

        result->setValues(data);
        result->setName(name);
        return result;
    }
}
