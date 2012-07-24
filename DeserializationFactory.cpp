#include "DeserializationFactory.h"
#include "core_UserDataType.h"
#include "Index.h"
#include "core_Table.h"
#include "core_Column.h"
#include "MajorVersion.h"
#include "Solution.h"
#include "db_DatabaseEngine.h"
#include "ForeignKey.h"
#include "dgram_DiagramTableDescriptor.h"
#include "dgram_DiagramNoteDescriptor.h"
#include "dgram_DiagramFKDescriptor.h"
#include "core_Diagram.h"
#include "core_TableInstance.h"
#include "core_View.h"
#include "qbr_SelectQuerySelectComponent.h"
#include "qbr_SelectQueryFromComponent.h"
#include "qbr_SelectQueryJoinComponent.h"
#include "qbr_SelectQueryWhereComponent.h"
#include "qbr_SelectQueryGroupByComponent.h"
#include "qbr_SelectQueryAsComponent.h"
#include "qbr_TableQueryComponent.h"
#include "qbr_SelectQueryOrderByComponent.h"
#include "qbr_SelectQuery.h"
#include "qbr_QueryComponents.h"
#include "qbr_SingleExpressionQueryComponent.h"
#include "qbr_DatabaseFunctionInstantiationComponent.h"
#include "Workspace.h"
#include "core_Procedure.h"
#include "core_Trigger.h"
#include "core_Function.h"
#include "uids.h"
#include "TrueFalseSp.h"
#include "SpInstance.h"

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
    QString isUnsigned = element.attribute("Unsigned");
    QString canBeNull = element.attribute("CanBeNull");
    QString description = "";
    QString uid = element.attribute("uid");
    QString class_uid = element.attribute("class-uid");
    QString source_uid = element.attribute("source-uid");

    if(uid.length() == 0)
    {
        uid = QUuid::createUuid().toString();
    }

    if(source_uid.length() == 0)
    {
        source_uid = nullUid;
    }

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

    UserDataType* result = new UserDataType(name, type, sqlType, size, defaultValue, miscValues, isUnsigned=="1", description, canBeNull == "1", QUuid::createUuid().toString());
    result->setSourceUid(source_uid);

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

Index* DeserializationFactory::createIndex(DatabaseEngine* engine, Table* table, const QDomDocument& doc, const QDomElement &element)
{
    QString uid = element.attribute("uid");
    QString class_uid = element.attribute("class-uid");
    QString name = element.attribute("Name");

    if(uid.length() == 0)
    {
        uid = QUuid::createUuid().toString();
    }
    if(class_uid != uidView)
    {
    }

    Index* result = new Index(name, table, uid);

    for(int i=0; i<element.childNodes().size(); i++)
    {
        if(element.childNodes().at(i).nodeName() == "Columns")
        {
            for(int j=0; j<element.childNodes().at(i).childNodes().size(); j++) // iterating through the "Column" nodes
            {
                QDomElement columnElement = element.childNodes().at(i).childNodes().at(j).toElement();
                QString columnName = columnElement.attribute("Name");
                QString order = columnElement.attribute("Order");
                result->addColumn(table->getColumn(columnName), order);    // finding the Name attribute
                if(columnElement.firstChild().toElement().nodeName() == "ColumnsSps")
                {
                    QDomElement colSpsElement = columnElement.firstChild().toElement();
                    for(int k=0; k<colSpsElement.childNodes().count(); k++)
                    {
                        QDomElement dbEngineSp = colSpsElement.childNodes().at(k).toElement();

                        QDomElement spiElement = dbEngineSp.childNodes().at(0).toElement();
                        QString spi_value = spiElement.attribute("Value");
                        QString spi_uid = spiElement.attribute("uid");

                        QDomElement sp = spiElement.firstChild().toElement();
                        QString sp_sql_role_uid = sp.attribute("sql-role-uid");

                        SpInstance* spi = createSpInstance(engine, sp_sql_role_uid, spi_uid);
                        if(spi)
                        {
                            spi->set(spi_value);
                            result->addSpToColumn(table->getColumn(columnName), dbEngineSp.attribute("Name"), spi);
                        }
                    }
                }
            }
        }
        if(element.childNodes().at(i).nodeName() == "SpInstances")
        {
            createObjectWithSpInstances(engine, result, doc, element.childNodes().at(i).firstChild().toElement());
        }
    }

    result->initializeRemainingSps(engine, QUuid(uidIndex));

    return result;
}

void DeserializationFactory::createMajorVersion(MajorVersion *mv, Project *p, DatabaseEngine *engine, const QDomDocument &doc, const QDomElement &element)
{
    QString name = "";

    int major = element.attribute("major").toInt();
    int minor = element.attribute("minor").toInt();

    for(int i=0; i<element.childNodes().count(); i++)
    {
        if(element.childNodes().at(i).nodeName() == "Version")
        {
            name = element.childNodes().at(i).firstChild().nodeValue();// first child is a text node
        }
    }

    mv->setVersionNumbersFromString(name);
    mv->setProject(p);
    mv->setVersionNumbers(major, minor);

    // getting the data types
    for(int i=0; i<element.childNodes().count(); i++)
    {
        if(element.childNodes().at(i).nodeName() == "DataTypes")
        {
            for(int j=0; j<element.childNodes().at(i).childNodes().count(); j++)
            {
                UserDataType* dt = createUserDataType(doc, element.childNodes().at(i).childNodes().at(j).toElement());
                mv->addNewDataType(dt);
            }
        }
    }

    // getting the tables
    for(int i=0; i<element.childNodes().count(); i++)
    {
        if(element.childNodes().at(i).nodeName() == "Tables")   // in a well formatted mv, the Tables child node is always after the datatypes
        {
            for(int j=0; j<element.childNodes().at(i).childNodes().count(); j++)
            {
                Table* tab = createTable(engine, mv, doc, element.childNodes().at(i).childNodes().at(j).toElement());
                mv->addTable(tab);
            }
        }
    }

    // update the parent child relationships of the table
    mv->setupTableParentChildRelationships();

    // now update the tables so that the foreign keys get populated
    for(int i=0; i<mv->getTables().size(); i++)
    {
        Table* tabI = mv->getTables().at(i);
        mv->setupForeignKeyRelationshipsForATable(tabI);
    }

    // now update the foreign keys of the tables so that the auto generated indices get populated properly
    for(int i=0; i<mv->getTables().size(); i++)
    {
        Table* tabI = mv->getTables().at(i);
        for(int j=0; j<tabI->getFks().size(); j++)
        {
            ForeignKey* fkJ = tabI->getFks()[j];
            fkJ->finalizeAutogeneratedIndices(mv);
        }
    }

    // getting the diagrams
    for(int i=0; i<element.childNodes().count(); i++)
    {
        if(element.childNodes().at(i).nodeName() == "Diagrams")   // in a well formatted mv, the Diagrams child node is always after the tables
        {
            for(int j=0; j<element.childNodes().at(i).childNodes().count(); j++)
            {
                Diagram* diagram = createDiagram(mv, doc, element.childNodes().at(i).childNodes().at(j).toElement());
                mv->addDiagram(diagram);
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
                TableInstance* tabInst = createTableInstance(mv, doc, element.childNodes().at(i).childNodes().at(j).toElement());
                mv->addTableInstance(tabInst);
            }

            // and here populate the "instantiated" like stuff
            for(int j=0; j<element.childNodes().at(i).childNodes().count(); j++)
            {
                createTableInstance(mv, doc, element.childNodes().at(i).childNodes().at(j).toElement(), true);
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
                View* view = createView(p, mv, doc, element.childNodes().at(i).childNodes().at(j).toElement());
                mv->addView(view);
            }
        }
    }

    // getting the procedures
    for(int i=0; i<element.childNodes().count(); i++)
    {
        if(element.childNodes().at(i).nodeName() == "Procedures")
        {
            for(int j=0; j<element.childNodes().at(i).childNodes().count(); j++)
            {
                Procedure* proc = createProcedure(p, mv, doc, element.childNodes().at(i).childNodes().at(j).toElement());
                mv->addProcedure(proc);
            }
        }
    }

    // getting the functions
    for(int i=0; i<element.childNodes().count(); i++)
    {
        if(element.childNodes().at(i).nodeName() == "Functions")
        {
            for(int j=0; j<element.childNodes().at(i).childNodes().count(); j++)
            {
                Function* func= createFunction(p, mv, doc, element.childNodes().at(i).childNodes().at(j).toElement());
                mv->addFunction(func);
            }
        }
    }

    // getting the triggers
    for(int i=0; i<element.childNodes().count(); i++)
    {
        if(element.childNodes().at(i).nodeName() == "Triggers")
        {
            for(int j=0; j<element.childNodes().at(i).childNodes().count(); j++)
            {
                Trigger* trig = createTrigger(p, mv, doc, element.childNodes().at(i).childNodes().at(j).toElement());
                mv->addTrigger(trig);
            }
        }
    }
}

QueryComponent* DeserializationFactory::createComponent(QueryComponent* parent, Project* p, Version* v, const QDomDocument &doc, const QDomElement &componentNode)
{
    QueryComponent* c = 0;

    QString strClass = componentNode.attribute("class");

    if(strClass == "SelectQuerySelectComponent")    // god, this is soo lame ... TODO: Find a more appropriate way to implement this mechanism
    {
        c = new SelectQuerySelectComponent(parent, componentNode.attribute("level").toInt());
    }

    if(strClass == "SelectQueryFromComponent")
    {
        c = new SelectQueryFromComponent(parent, componentNode.attribute("level").toInt());
    }

    if(strClass == "SelectQueryWhereComponent")
    {
        c = new SelectQueryWhereComponent(parent, componentNode.attribute("level").toInt(), static_cast<SelectQueryWhereComponent::WhereType>(componentNode.attribute("Type").toInt()));
    }

    if(strClass == "SelectQueryHavingComponent")
    {
        c = new SelectQueryWhereComponent(parent, componentNode.attribute("level").toInt(), static_cast<SelectQueryWhereComponent::WhereType>(componentNode.attribute("Type").toInt()));
    }

    if(strClass == "SelectQueryGroupByComponent")
    {
        c = new SelectQueryGroupByComponent(parent, componentNode.attribute("level").toInt());
    }

    if(strClass == "SelectQueryOrderByComponent")
    {
        c = new SelectQueryOrderByComponent(parent, componentNode.attribute("level").toInt());
    }

    if(strClass == "SelectQueryJoinComponent")
    {
        c = new SelectQueryJoinComponent(parent, componentNode.attribute("level").toInt());
    }

    if(strClass == "SingleExpressionQueryComponent")
    {
         c = new SingleExpressionQueryComponent(parent, componentNode.attribute("level").toInt());    // This is <Expression>
         dynamic_cast<SingleExpressionQueryComponent*>(c)->setForcedType((SingleExpressionQueryComponent::ForcedSingleExpressionQueryComponent)componentNode.attribute("forced").toInt(), true);

         if(componentNode.hasAttribute("alias"))
         {
             QString a = componentNode.attribute("alias");
             SelectQueryAsComponent* sqas = new SelectQueryAsComponent(c, c->getLevel() + 1);
             sqas->setAs(a);
             dynamic_cast<SingleExpressionQueryComponent*>(c)->setAs(sqas);
         }

         for(int i=0; i<componentNode.childNodes().count(); i++)
         {
             QDomElement element = componentNode.childNodes().at(i).toElement(); // this is <Element>
             int idx =  element.attribute("idx").toInt();
             int type = element.attribute("Type").toInt();
             dynamic_cast<SingleExpressionQueryComponent*>(c)->addElement(static_cast<CellTypeChooserType>(type));
             if(type == CELLTYPE_COLUMN)
             {
                 QString col = element.attribute("Column");
                 QString tab = element.attribute("Table");
                 Table* t = v->getTable(tab);
                 if(!t)
                 {
                     return 0;
                 }
                 Column* tcol = t->getColumn(col);
                 if(tcol == 0) tcol = t->getColumnFromParents(col);
                 if(tcol == 0) return 0;
                 dynamic_cast<SingleExpressionQueryComponent*>(c)->setColumnAtGivenPosition(idx, tcol);
             }
             if(type == CELLTYPE_LITERAL)
             {
                 QString str = element.attribute("InputString");
                 dynamic_cast<SingleExpressionQueryComponent*>(c)->setTextAtGivenPosition(idx, str);
             }
             if(type == CELLTYPE_FUNCTION)
             {
                 QDomElement function = element.firstChild().firstChild().toElement();  // <Function>
                 QString name = function.attribute("Name");
                 const DatabaseBuiltinFunction& f = p->getEngine()->getBuiltinFunction(name.mid(1));
                 DatabaseFunctionInstantiationComponent* fic = new DatabaseFunctionInstantiationComponent(c, f, true);
                 QDomElement parameters = function.firstChild().toElement();        // <Parameters>
                 for(int j=0; j<parameters.childNodes().count(); j++)
                 {
                     QDomElement parameterElement = parameters.childNodes().at(j).toElement();
                     QueryComponent* parameter = createComponent(c, p, v, doc, parameterElement.firstChild().toElement());
                     fic->addParameter(dynamic_cast<SingleExpressionQueryComponent*>(parameter));
                 }

                 dynamic_cast<SingleExpressionQueryComponent*>(c)->setFunctionInstantiationAtGivenPosition(idx, fic, &f);
             }
         }
    }

    if(strClass == "TableQueryComponent")
    {
        Table* t = v->getTable(componentNode.attribute("Name"));
        if(t == 0) return 0;

        c = new TableQueryComponent(t, parent, componentNode.attribute("level").toInt());
        if(componentNode.hasAttribute("As"))
        {
            QString a = componentNode.attribute("As");
            SelectQueryAsComponent* sqas = new SelectQueryAsComponent(c, c->getLevel() + 1);
            sqas->setAs(a);
            dynamic_cast<TableQueryComponent*>(c)->setAs(sqas);
        }

        if(componentNode.hasChildNodes())   // see if we have a joins
        {
            for(int i=0; i<componentNode.childNodes().count(); i++)
            {
                if(componentNode.childNodes().at(i).nodeName() == "Joins")
                {
                    for(int j=0; j<componentNode.childNodes().at(i).childNodes().count(); j++)
                    {
                        SelectQueryJoinComponent* sqjc = dynamic_cast<SelectQueryJoinComponent*> (createComponent(c, p, v, doc, componentNode.childNodes().at(i).childNodes().at(j).toElement()));
                        dynamic_cast<TableQueryComponent*>(c)->addJoin(sqjc);
                    }
                }
            }
        }
    }

    for(int k=0; k<componentNode.childNodes().count(); k++)
    {
        if(componentNode.childNodes().at(k).nodeName() == "Children")
        {
            for(int i=0; i<componentNode.childNodes().at(k).childNodes().count(); i++)
            {
                QDomElement e = componentNode.childNodes().at(k).childNodes().at(i).toElement();    // This is <Child idx="12">
                int idx = e.attribute("idx").toInt();
                QueryComponent* child = createComponent(c, p, v, doc, e.firstChild().toElement()); // This is "Expression"
                c->setChild(child, idx);
            }
        }
        if(componentNode.childNodes().at(k).nodeName() == "On" && strClass == "SelectQueryJoinComponent")
        {
            for(int i=0; i<componentNode.childNodes().at(k).childNodes().count(); i++)
            {
                QDomElement e = componentNode.childNodes().at(k).childNodes().at(i).toElement();    // This is <JoinExpression idx="12">
                QueryComponent* child = createComponent(c, p, v, doc, e.firstChild().toElement()); // This is "Expression"
                dynamic_cast<SelectQueryJoinComponent*>(c)->addJoinExpression(dynamic_cast<SingleExpressionQueryComponent*>(child));
            }
        }
    }

    if(componentNode.hasAttribute("uid"))
    {
        QString s = componentNode.attribute("uid");
        c->setForcedUid(s);
    }
    else
    {
        c->setForcedUid(QUuid::createUuid().toString());
    }

    return c;
}

View* DeserializationFactory::createView(Project* p, Version* v, const QDomDocument& doc, const QDomElement& element)
{
    bool manual = element.attribute("Manual") == "1";
    QString uid = element.attribute("uid");
    QString class_uid = element.attribute("class-uid");
    QString name = element.attribute("Name");

    if(uid.length() == 0)
    {
        uid = QUuid::createUuid().toString();
    }
    if(class_uid != uidView)
    {
    }
    View* view = new View(v, manual, uid);
    view->setName(name);

    if(manual)
    {
        QDomElement sqlElement = element.firstChild().toElement();
        QDomCDATASection cdata = sqlElement.firstChild().toCDATASection();
        view->setSql(cdata.toText().data());
    }
    else
    {
        for(int i=0; i<element.childNodes().count(); i++)
        {
            if(element.childNodes().at(i).nodeName() == "Columns")
            {
                QDomElement columnsNode = element.childNodes().at(i).toElement();
                QStringList columnNames;
                for(int j=0; j< columnsNode.childNodes().count(); j++)
                {
                    QString colName = columnsNode.childNodes().at(j).toElement().attribute("Name");
                    columnNames.append(colName);
                }
                view->setColumnNames(columnNames);
            }
            if(element.childNodes().at(i).nodeName() == "Query")
            {
                QDomElement queryNode = element.childNodes().at(i).toElement();
                if(queryNode.hasAttribute("class-uid"))
                {
                    QString cuid = queryNode.attribute("class-uid");
                    if(cuid.toUpper() != uidSelectQuery)
                    {
                        qDebug() << "Views's select query UID is not valid: cuid (" << cuid << ") != " << uidSelectQuery ;
                    }
                }
                SelectQuery* q = new SelectQuery(view->getHelper(), view);
                if(queryNode.hasAttribute("uid"))
                {
                    q->setForcedUid(queryNode.attribute("uid"));
                }
                view->setQuery(q);
                view->getHelper()->setQuery(q);

                for(int j=0; j< queryNode.childNodes().count(); j++)
                {
                    QDomElement childNode = queryNode.childNodes().at(j).toElement();
                    if(childNode.nodeName() == "Select")
                    {
                        SelectQuerySelectComponent* sqsc = dynamic_cast<SelectQuerySelectComponent*> (createComponent(q, p, v, doc, childNode));
                        q->setSelect(sqsc);
                    }
                    if(childNode.nodeName() == "From")
                    {
                        SelectQueryFromComponent* sqfc = dynamic_cast<SelectQueryFromComponent*> (createComponent(q, p, v, doc, childNode));
                        q->setFrom(sqfc);
                    }
                    if(childNode.nodeName() == "WhereComponent")
                    {
                        SelectQueryWhereComponent* sqwc = dynamic_cast<SelectQueryWhereComponent*> (createComponent(q, p, v, doc, childNode));
                        q->setWhere(sqwc);
                    }
                    if(childNode.nodeName() == "GroupBy")
                    {
                        SelectQueryGroupByComponent* sqgbc = dynamic_cast<SelectQueryGroupByComponent*> (createComponent(q, p, v, doc, childNode));
                        q->setGroupBy(sqgbc);
                    }
                    if(childNode.nodeName() == "HavingComponent")
                    {
                        SelectQueryWhereComponent* sqwc = dynamic_cast<SelectQueryWhereComponent*> (createComponent(q, p, v, doc, childNode));
                        q->setHaving(sqwc);
                    }
                    if(childNode.nodeName() == "OrderBy")
                    {
                        SelectQueryOrderByComponent* sqobc = dynamic_cast<SelectQueryOrderByComponent*> (createComponent(q, p, v, doc, childNode));
                        q->setOrderBy(sqobc);
                    }
                }
            }
        }
    }

    return view;
}

Column* DeserializationFactory::createColumn(DatabaseEngine* engine, Version* ver, const QDomDocument &doc, const QDomElement &element)
{
    QString name = element.attribute("Name");
    QString pk = element.attribute("PK");
    QString type = element.attribute("Type");
    UserDataType* udt = ver->getDataType(type);
    QString uid = element.attribute("uid");
    QString class_uid = element.attribute("class-uid");
    if(uid.length() == 0)
    {
        uid = QUuid::createUuid().toString();
    }
    if(class_uid != uidColumn)
    {

    }

    Column* col = new Column(uid, name, udt, pk == "1");

    for(int i=0; i<element.childNodes().count(); i++)
    {
        QDomElement cNode = element.childNodes().at(i).toElement();
        if(cNode.nodeName() == "Description")
        {
            QString g = cNode.firstChild().nodeValue();
            col->setDescription(g);
        }

        if(cNode.nodeName() == "SpInstances")
        {
            createObjectWithSpInstances(engine, col, doc, cNode.firstChild().toElement());
        }
    }

    col->initializeRemainingSps(engine, QUuid(uidColumn));

    return col;
}

Table* DeserializationFactory::createTable(DatabaseEngine* engine, Version* ver, const QDomDocument &doc, const QDomElement &element)
{
    QString name = element.attribute("Name");
    QString uid = element.attribute("uid");
    QString class_uid = element.attribute("class-uid");

    if(class_uid != uidTable)
    {
    }

    if(uid.length() == 0)
    {
        uid = QUuid::createUuid();
    }

    QString parent_uid = element.attribute("parent-uid");

    Table* result = new Table(ver, uid, 0);

    result->setParentUid(parent_uid);
    result->setName(name);
    result->setTempTabName(element.attribute("Parent"));

    for(int i=0; i<element.childNodes().count(); i++)
    {
        QString nodeN = element.childNodes().at(i).nodeName();
        if(nodeN == "Description")
        {
            QString d = element.childNodes().at(i).firstChild().nodeValue();
            result->setDescription(d);
        }
        if(nodeN == "Columns")
        {
            for(int j=0; j<element.childNodes().at(i).childNodes().count(); j++)
            {
                Column* col = createColumn(engine, ver, doc, element.childNodes().at(i).childNodes().at(j).toElement());
                result->addColumn(col);
            }
        }
        if(nodeN == "Indices")
        {
            for(int j=0; j<element.childNodes().at(i).childNodes().count(); j++)
            {
                Index* idx = createIndex(engine, result, doc, element.childNodes().at(i).childNodes().at(j).toElement());
                result->addIndex(idx);
            }
        }
        if(nodeN == "ForeignKeys")
        {
            for(int j=0; j<element.childNodes().at(i).childNodes().count(); j++)
            {
                ForeignKey* fk = createForeignKey(result, doc, element.childNodes().at(i).childNodes().at(j).toElement());
                result->addForeignKey(fk);
            }
        }
        if(nodeN == "StartupValues")
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

        if(nodeN == "SpInstances")
        {
            createObjectWithSpInstances(engine, result, doc, element.childNodes().at(i).firstChild().toElement());
        }
    }

    result->initializeRemainingSps(engine, QUuid(uidTable));

    return result;
}

void DeserializationFactory::createObjectWithSpInstances(DatabaseEngine* engine, ObjectWithSpInstances *obj, const QDomDocument &/*doc*/, const QDomElement &element)
{
    QDomElement dbEnginesSps = element;
    for(int j=0; j<dbEnginesSps.childNodes().count(); j++)
    {
        QDomElement dbEngineSp = dbEnginesSps.childNodes().at(j).toElement();
        if(dbEngineSp.attribute("Name") == engine->getDatabaseEngineName())
        {
            for(int k=0; k<dbEngineSp.childNodes().count(); k++)
            {
                QDomElement spiElement = dbEngineSp.childNodes().at(k).toElement();
                QString spi_value = spiElement.attribute("Value");
                QString spi_uid = spiElement.attribute("uid");
                QString spi_class_uid = spiElement.attribute("class-uid");

                QDomElement sp = spiElement.firstChild().toElement();
                QString sp_sql_role_uid = sp.attribute("sql-role-uid");
                QString sp_class_uid = sp.attribute("class-uid");
                QString sp_uid = sp.attribute("uid");
                QString sp_name = sp.attribute("Name");
                QString sp_referred_object_class_uid = sp.attribute("referred-object-class-uid");

                SpInstance* spi = createSpInstance(engine, sp_sql_role_uid, spi_uid);
                if(spi)
                {
                    spi->set(spi_value);
                    obj->addSpInstance(engine, spi);
                }
            }
        }
    }
}

SpInstance* DeserializationFactory::createSpInstance(DatabaseEngine* engine, const QString &sql_role_uid, const QString& spi_uid)
{
    Sp* sp = engine->getSpForSqlRole(sql_role_uid);
    if(sp)
    {
        SpInstance* spi = sp->createSpecifiedInstance(spi_uid);
        return spi;
    }
    return 0;
}

void DeserializationFactory::createProject(Project *project, const QDomDocument &doc, const QDomElement &element)
{
    project->setName(element.attribute("Name"));
    project->setOop(element.attribute("OOP")=="1");
    DatabaseEngine* engine = DatabaseEngine::provideEngineFor(element.attribute("DB"));
    project->setEngine(engine);

    for(int i=0; i<element.childNodes().count(); i++)
    {
        if(element.childNodes().at(i).nodeName() == "MajorVersions")
        {
            QDomNodeList majorVersionNodes = element.childNodes().at(i).childNodes();
            for(int i=0; i<majorVersionNodes.count(); i++)
            {
                MajorVersion* majVer = new MajorVersion();
                project->addMajorVersion(majVer);
                createMajorVersion(majVer, project, engine, doc, majorVersionNodes.at(i).toElement());
            }
        }
        else
        if(element.childNodes().at(i).nodeName() == "Description")
        {
            project->setDescription(element.childNodes().at(i).firstChild().nodeValue());
        }
    }
}

void DeserializationFactory::createSolution(Solution *s, const QDomDocument &doc, const QDomElement &element)
{
    s->setName(element.attribute("Name"));

    QDomNodeList projectElements = element.firstChild().childNodes();
    for(int i=0; i<projectElements.count(); i++) //
    {
        Project* prj = new Project();
        s->addProject(prj);

        createProject(prj, doc, projectElements.at(i).toElement());
    }
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
    QString uid = element.attribute("uid");
    QString class_uid = element.attribute("class-uid");

    if(uid.length() == 0)
    {
        uid = QUuid::createUuid().toString();
    }

    if(class_uid != uidDiagram)
    {
    }

    Diagram* result = new Diagram(v, name, uid);

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
    QString uid = element.attribute("uid");
    QString class_uid = element.attribute("class-uid");
    if(uid.length() == 0)
    {
        uid = QUuid::createUuid().toString();
    }

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
        TableInstance* result = new TableInstance(v->getTable(tabName), refed, uid);

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

Procedure* DeserializationFactory::createProcedure(Project*, Version*,  const QDomDocument&, const QDomElement& element)
{
    QString name = element.attribute("Name");
    QString uid = element.attribute("uid");
    QString class_uid = element.attribute("class-uid");

    if(uid.length() == 0)
    {
        uid = QUuid::createUuid().toString();
    }
    if(class_uid != uidProcedure)
    {

    }
    Procedure* p = new Procedure(name, uid);

    QDomElement sqlElement = element.firstChild().toElement();
    if(sqlElement.hasAttribute("Encoded"))
    {
        if(sqlElement.attribute("Encoded") == "Base64")
        {
            QDomCDATASection cdata = sqlElement.firstChild().toCDATASection();
            QByteArray encoded = QByteArray(cdata.toText().data().toLocal8Bit());
            p->setSql(QString(QByteArray::fromBase64(encoded)));

            return p;
        }
    }

    // assuming a not encoded SQL, old versions, pre 0.1i
    QDomCDATASection cdata = sqlElement.firstChild().toCDATASection();
    p->setSql(cdata.toText().data().toLocal8Bit());
    return p;
}

Function* DeserializationFactory::createFunction(Project*, Version*,  const QDomDocument&, const QDomElement& element)
{
    QString name = element.attribute("Name");
    QString uid = element.attribute("uid");
    QString class_uid = element.attribute("class-uid");
    if(uid.length() == 0)
    {
        uid = QUuid::createUuid().toString();
    }
    if(class_uid != uidFunction)
    {

    }
    Function* p = new Function(name, uid);
    QDomElement sqlElement = element.firstChild().toElement();
    if(sqlElement.hasAttribute("Encoded"))
    {
        if(sqlElement.attribute("Encoded") == "Base64")
        {
            QDomCDATASection cdata = sqlElement.firstChild().toCDATASection();
            QByteArray encoded = QByteArray(cdata.toText().data().toLocal8Bit());
            p->setSql(QString(QByteArray::fromBase64(encoded)));

            return p;
        }
    }

    // assuming a not encoded SQL, old versions, pre 0.1i
    QDomCDATASection cdata = sqlElement.firstChild().toCDATASection();
    p->setSql(cdata.toText().data());
    return p;
}

Trigger* DeserializationFactory::createTrigger(Project*, Version* v,  const QDomDocument&, const QDomElement& element)
{
    Table* tab = v->getTable(element.attribute("Table"));
    if(tab == 0) return 0;
    QString name = element.attribute("Name");
    QString uid = element.attribute("uid");
    if(uid.length() == 0)
    {
        uid = QUuid::createUuid().toString();
    }
    QString class_uid = element.attribute("class-uid");

    Trigger* trigg = new Trigger(name, uid);
    for(int i=0; i<element.childNodes().size(); i++)
    {
        if(element.childNodes().at(i).nodeName() == "Body")
        {
            QDomElement sqlElement = element.childNodes().at(i).toElement();
            if(sqlElement.hasAttribute("Encoded"))
            {
                if(sqlElement.attribute("Encoded") == "Base64")
                {
                    QDomCDATASection cdata = sqlElement.firstChild().toCDATASection();
                    QByteArray encoded = QByteArray(cdata.toText().data().toLocal8Bit());
                    trigg->setSql(QString(QByteArray::fromBase64(encoded)));
                }
            }
            else
            {
                QDomText cdata = sqlElement.firstChild().toText();
                trigg->setSql(cdata.data());
            }
        }

        if(element.childNodes().at(i).nodeName() == "Description")
        {
            QDomElement descElement = element.childNodes().at(i).toElement();
            QDomText cdata = descElement.firstChild().toText();
            trigg->setDescription(cdata.data());
        }
    }

    trigg->setTime(element.attribute("Time"));
    trigg->setEvent(element.attribute("Event"));
    trigg->setTable(tab->getName());
    return trigg;
}
