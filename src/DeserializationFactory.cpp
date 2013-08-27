#include "DeserializationFactory.h"
#include "core_UserDataType.h"
#include "core_Index.h"
#include "core_Table.h"
#include "core_Column.h"
#include "MajorVersion.h"
#include "Solution.h"
#include "db_DatabaseEngine.h"
#include "core_ForeignKey.h"
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
#include "core_Patch.h"
#include "db_DatabaseEngineManager.h"

#include <QStringList>

DeserializationFactory::DeserializationFactory()
{
}

UserDataType* DeserializationFactory::createUserDataType(Version* v, const QDomDocument&, const QDomElement& element)
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
    QString locked = element.attribute("locked");
    QString wasLocked = element.attribute("was-locked");

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

    UserDataType* result = new UserDataType(name, type, sqlType, size, defaultValue, miscValues, isUnsigned=="1", description, canBeNull == "1", QUuid::createUuid().toString(), v);

    if(source_uid.length())
    {
        result->setSourceUid(source_uid);
    }
    else
    {
        result->setSourceUid(nullUid);
    }
    if(locked == "1")
    {
        result->lock(LockableElement::LOCKED);
    }
    else
    {
        result->unlock();
    }

    result->forceSetWasLocked(wasLocked == "1");

    return result;

}

ForeignKey* DeserializationFactory::createForeignKey(Table *t, const QDomDocument &, const QDomElement &element)
{
    QString name = element.attribute("Name");
    QString onUpdate = element.attribute("OnUpdate");
    QString onDelete = element.attribute("OnDelete");
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

    ForeignKey* fk = new ForeignKey(t->version(), uid);
    fk->setName(name);
    fk->setOnDelete(onDelete);
    fk->setOnUpdate(onUpdate);
    fk->setSourceUid(source_uid);

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

Index* DeserializationFactory::createIndex(DatabaseEngine* engine, Table* table, const QDomDocument& doc, const QDomElement &element, Version* v)
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

    Index* result = new Index(name, table, uid, v);

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
    QString uid = element.attribute("uid");
    if(uid.length() == 0)
    {
        uid = QUuid::createUuid().toString();
    }
    mv->setForcedUid(uid);
    QString sourceUid = element.attribute("source-uid");
    if(sourceUid.length())
    {
        mv->setSourceUid(QUuid(sourceUid));
    }
    else
    {
        mv->setSourceUid(nullUid);
    }
    QString classUid = element.attribute("class-uid");
    Q_UNUSED(classUid);

    QString locked = element.attribute("locked");
    if(locked.length())
    {
        if(locked == "1")
        {
            mv->lock(LockableElement::LOCKED);
        }
        else
        {
            mv->unlock();
        }
    }

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
                UserDataType* dt = createUserDataType(mv, doc, element.childNodes().at(i).childNodes().at(j).toElement());
                mv->addNewDataType(dt, true);
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
                mv->addTable(tab, true);
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
        for(int j=0; j<tabI->getForeignKeys().size(); j++)
        {
            ForeignKey* fkJ = tabI->getForeignKeys()[j];
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
                mv->addDiagram(diagram, true);
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
                mv->addTableInstance(tabInst, true);
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
                View* view = createView(mv, engine, doc, element.childNodes().at(i).childNodes().at(j).toElement());
                mv->addView(view, true);
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
                Procedure* proc = createProcedure(mv, doc, element.childNodes().at(i).childNodes().at(j).toElement());
                mv->addProcedure(proc, true);
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
                Function* func= createFunction(mv, doc, element.childNodes().at(i).childNodes().at(j).toElement());
                mv->addFunction(func, true);
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
                Trigger* trig = createTrigger(mv, doc, element.childNodes().at(i).childNodes().at(j).toElement());
                mv->addTrigger(trig, true);
            }
        }
    }
    // getting the patches ... if any
    for(int i=0; i<element.childNodes().count(); i++)
    {
        if(element.childNodes().at(i).nodeName() == "Patches")
        {
            for(int j=0; j<element.childNodes().at(i).childNodes().count(); j++)
            {
                Patch* patch = createPatch(p, mv, doc, element.childNodes().at(i).childNodes().at(j).toElement());
                mv->addPatch(patch);
                patch->finalizePatchDeserialization();
            }
        }
    }
}

QueryComponent* DeserializationFactory::createComponent(Query* query, QueryComponent* parent, Project* p, Version* v, const QDomDocument &doc, const QDomElement &componentNode)
{
    QueryComponent* c = 0;

    QString strClass = componentNode.attribute("class");

    if(strClass == "SelectQuerySelectComponent")    // god, this is soo lame ... TODO: Find a more appropriate way to implement this mechanism
    {
        c = new SelectQuerySelectComponent(query, parent, componentNode.attribute("level").toInt(), v);
    }

    if(strClass == "SelectQueryFromComponent")
    {
        c = new SelectQueryFromComponent(query, parent, componentNode.attribute("level").toInt(), v);
    }

    if(strClass == "SelectQueryWhereComponent")
    {
        c = new SelectQueryWhereComponent(query, parent, componentNode.attribute("level").toInt(),
                                          static_cast<SelectQueryWhereComponent::WhereType>(componentNode.attribute("Type").toInt()),
                                          v);
    }

    if(strClass == "SelectQueryHavingComponent")
    {
        c = new SelectQueryWhereComponent(query, parent, componentNode.attribute("level").toInt(),
                                          static_cast<SelectQueryWhereComponent::WhereType>(componentNode.attribute("Type").toInt()),
                                          v);
    }

    if(strClass == "SelectQueryGroupByComponent")
    {
        c = new SelectQueryGroupByComponent(query, parent, componentNode.attribute("level").toInt(), v);
    }

    if(strClass == "SelectQueryOrderByComponent")
    {
        c = new SelectQueryOrderByComponent(query, parent, componentNode.attribute("level").toInt(), v);
    }

    if(strClass == "SelectQueryJoinComponent")
    {
        c = new SelectQueryJoinComponent(query, parent, componentNode.attribute("level").toInt(), v);
    }

    if(strClass == "SingleExpressionQueryComponent")
    {
         c = new SingleExpressionQueryComponent(query, parent, componentNode.attribute("level").toInt(), v);    // This is <Expression>
         dynamic_cast<SingleExpressionQueryComponent*>(c)->setForcedType((SingleExpressionQueryComponent::ForcedSingleExpressionQueryComponent)componentNode.attribute("forced").toInt(), true);

         if(componentNode.hasAttribute("alias"))
         {
             QString a = componentNode.attribute("alias");
             SelectQueryAsComponent* sqas = new SelectQueryAsComponent(query, c, c->nextLevel(), v);
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
                 QString tabInstance = element.attribute("TabInstance");
                 Table* t = v->getTable(tab);
                 if(!t)
                 {
                     return 0;
                 }
                 TableInstance* tinst = v->getTableInstance(tabInstance);
                 Column* tcol = t->getColumn(col);
                 if(tcol == 0) tcol = t->getColumnFromParents(col);
                 if(tcol == 0) return 0;
                 ColumnOfTabWithTabInstance* coft = new ColumnOfTabWithTabInstance;
                 coft->c = tcol;
                 coft->tab = t;
                 coft->tinst = tinst;
                 dynamic_cast<SingleExpressionQueryComponent*>(c)->setColumnAtGivenPosition(idx, coft);
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
                 DatabaseFunctionInstantiationComponent* fic = new DatabaseFunctionInstantiationComponent(query, c, f, true, v);
                 QDomElement parameters = function.firstChild().toElement();        // <Parameters>
                 for(int j=0; j<parameters.childNodes().count(); j++)
                 {
                     QDomElement parameterElement = parameters.childNodes().at(j).toElement();
                     QueryComponent* parameter = createComponent(query, c, p, v, doc, parameterElement.firstChild().toElement());
                     fic->addParameter(dynamic_cast<SingleExpressionQueryComponent*>(parameter));
                 }

                 dynamic_cast<SingleExpressionQueryComponent*>(c)->setFunctionInstantiationAtGivenPosition(idx, fic, &f);
             }
         }
    }

    if(strClass == "TableQueryComponent")
    {
        QString tName = componentNode.attribute("Name");

        bool tabInsteadOfTinst = false;
        if(componentNode.hasAttribute("tabInstedOfTinst"))
        {
            tabInsteadOfTinst = componentNode.attribute("tabInstedOfTinst") == "1";
        }
        if(tabInsteadOfTinst)
        {
            Table* table = v->getTable(tName);
            if(table == 0) return 0;
            c = new TableQueryComponent(query, table, parent, componentNode.attribute("level").toInt(), v);
        }
        else
        {
            TableInstance* tinst = v->getTableInstance(tName);
            if(tinst == 0) return 0;
            c = new TableQueryComponent(query, tinst, parent, componentNode.attribute("level").toInt(), v);
        }
        if(componentNode.hasAttribute("As"))
        {
            QString a = componentNode.attribute("As");
            SelectQueryAsComponent* sqas = new SelectQueryAsComponent(query, c, c->nextLevel(), v);
            sqas->setAs(a);
            dynamic_cast<TableQueryComponent*>(c)->setAs(sqas);

            query->setQueryGlobalAlias(sqas, dynamic_cast<TableQueryComponent*>(c));
        }

        if(componentNode.hasChildNodes())   // see if we have a joins
        {
            for(int i=0; i<componentNode.childNodes().count(); i++)
            {
                if(componentNode.childNodes().at(i).nodeName() == "Joins")
                {
                    for(int j=0; j<componentNode.childNodes().at(i).childNodes().count(); j++)
                    {
                        SelectQueryJoinComponent* sqjc = dynamic_cast<SelectQueryJoinComponent*> (createComponent(query, c, p, v, doc, componentNode.childNodes().at(i).childNodes().at(j).toElement()));
                        dynamic_cast<TableQueryComponent*>(c)->addJoin(sqjc);
                    }
                }
            }
        }
    }

    if(!c) return 0;

    for(int k=0; k<componentNode.childNodes().count(); k++)
    {
        if(componentNode.childNodes().at(k).nodeName() == "Children")
        {
            for(int i=0; i<componentNode.childNodes().at(k).childNodes().count(); i++)
            {
                QDomElement e = componentNode.childNodes().at(k).childNodes().at(i).toElement();    // This is <Child idx="12">
                int idx = e.attribute("idx").toInt();
                QueryComponent* child = createComponent(query, c, p, v, doc, e.firstChild().toElement()); // This is "Expression"
                if(child)
                {
                    c->setChild(child, idx);
                }
            }
        }
        if(componentNode.childNodes().at(k).nodeName() == "On" && strClass == "SelectQueryJoinComponent")
        {
            for(int i=0; i<componentNode.childNodes().at(k).childNodes().count(); i++)
            {
                QDomElement e = componentNode.childNodes().at(k).childNodes().at(i).toElement();    // This is <JoinExpression idx="12">
                QueryComponent* child = createComponent(query, c, p, v, doc, e.firstChild().toElement()); // This is "Expression"
                if(child)
                {
                    dynamic_cast<SelectQueryJoinComponent*>(c)->addJoinExpression(dynamic_cast<SingleExpressionQueryComponent*>(child));
                }
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

View* DeserializationFactory::createView(Version* v, DatabaseEngine* engine, const QDomDocument& doc, const QDomElement& element)
{
    bool manual = element.attribute("Manual") == "1";
    QString uid = element.attribute("uid");
    QString class_uid = element.attribute("class-uid");
    QString name = element.attribute("Name");

    QString source_uid = element.attribute("source-uid");
    QString locked = element.attribute("locked");
    QString wasLocked = element.attribute("was-locked");

    if(source_uid.length() == 0)
    {
        source_uid = nullUid;
    }

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

            if(element.childNodes().at(i).nodeName() == "SpInstances")
            {
                createObjectWithSpInstances(engine, view, doc, element.childNodes().at(i).firstChild().toElement());
            }

            if(element.childNodes().at(i).nodeName() == "Query")
            {
                QDomElement queryNode = element.childNodes().at(i).toElement();
                if(queryNode.hasAttribute("class-uid"))
                {
                    QString cuid = queryNode.attribute("class-uid");
                    if(cuid.toUpper() != uidSelectQuery)
                    {
//                        qDebug() << "Views's select query UID is not valid: cuid (" << cuid << ") != " << uidSelectQuery ;
                    }
                }
                SelectQuery* q = new SelectQuery(view->getHelper(), view, v);
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
                        SelectQuerySelectComponent* sqsc = dynamic_cast<SelectQuerySelectComponent*> (createComponent(q, q, v->getProject(), v, doc, childNode));
                        q->setSelect(sqsc);
                    }
                    if(childNode.nodeName() == "From")
                    {
                        SelectQueryFromComponent* sqfc = dynamic_cast<SelectQueryFromComponent*> (createComponent(q, q, v->getProject(), v, doc, childNode));
                        q->setFrom(sqfc);
                    }
                    if(childNode.nodeName() == "WhereComponent")
                    {
                        SelectQueryWhereComponent* sqwc = dynamic_cast<SelectQueryWhereComponent*> (createComponent(q, q, v->getProject(), v, doc, childNode));
                        q->setWhere(sqwc);
                    }
                    if(childNode.nodeName() == "GroupBy")
                    {
                        SelectQueryGroupByComponent* sqgbc = dynamic_cast<SelectQueryGroupByComponent*> (createComponent(q, q, v->getProject(), v, doc, childNode));
                        q->setGroupBy(sqgbc);
                    }
                    if(childNode.nodeName() == "HavingComponent")
                    {
                        SelectQueryWhereComponent* sqwc = dynamic_cast<SelectQueryWhereComponent*> (createComponent(q, q, v->getProject(), v, doc, childNode));
                        q->setHaving(sqwc);
                    }
                    if(childNode.nodeName() == "OrderBy")
                    {
                        SelectQueryOrderByComponent* sqobc = dynamic_cast<SelectQueryOrderByComponent*> (createComponent(q, q, v->getProject(), v, doc, childNode));
                        q->setOrderBy(sqobc);
                    }
                }
            }
        }
    }

    view->setSourceUid(source_uid);

    if(locked == "1")
    {
        view->lock(LockableElement::LOCKED);
    }
    else
    {
        view->unlock();
    }

    view->forceSetWasLocked(wasLocked == "1");
    view->initializeRemainingSps(engine, QUuid(uidView));

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
    QString source_uid = element.attribute("source-uid");
    if(source_uid.length() == 0)
    {
        source_uid = nullUid;
    }

    Column* col = new Column(uid, name, udt, pk == "1", ver);

    col->setSourceUid(source_uid);

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

    Table* result = new Table(ver, uid);

    QString locked = element.attribute("locked");
    if(locked == "1")
    {
        result->lock(LockableElement::LOCKED);
    }
    else
    {
        result->unlock();
    }
    QString sourceUid = element.attribute("source-uid");
    if(sourceUid.length())
    {
        result->setSourceUid(sourceUid);
    }
    else
    {
        result->setSourceUid(nullUid);
    }

    QString wasLocked = element.attribute("was-locked");
    result->forceSetWasLocked(wasLocked == "1");

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
                Index* idx = createIndex(engine, result, doc, element.childNodes().at(i).childNodes().at(j).toElement(), ver);
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
    DatabaseEngine* engine = DatabaseEngineManager::instance().engine(element.attribute("DB"));
    project->setEngine(engine);
    int wvi = element.attribute("WorkingVersionIndex").toInt();

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

    if(wvi > 0)
    {
        project->setWorkingVersionIndex(wvi);
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

DiagramTableDescriptor* DeserializationFactory::createDiagramTableDescriptor(const QDomDocument &, const QDomElement &element, Version* v)
{
    QString tabName = element.attribute("Name");
    int x = element.attribute("x").toInt();
    int y = element.attribute("y").toInt();

    DiagramTableDescriptor* result = new DiagramTableDescriptor(tabName, x, y, v);
    return result;
}

DiagramNoteDescriptor* DeserializationFactory::createDiagramNoteDescriptor(const QDomDocument &, const QDomElement &element, Version* v)
{
    int x = element.attribute("x").toInt();
    int y = element.attribute("y").toInt();
    bool framed = element.attribute("frame")=="1";
    QDomElement textNode = element.firstChild().toElement();
    QDomNode cdataNode = textNode.firstChild();
    QString text = cdataNode.nodeValue();

    DiagramNoteDescriptor* result = new DiagramNoteDescriptor(text, x, y, framed, v);
    return result;
}

DiagramFKDescriptor* DeserializationFactory::createDiagramFKDescriptor(const QDomDocument&, const QDomElement& element, Version* v)
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
                                                           arrowp1x,  arrowp1y,  arrowp2x,  arrowp2y,  arrowposx,  arrowposy, fkn, v);

    return dfk;

}

Diagram* DeserializationFactory::createDiagram(Version* v, const QDomDocument &doc, const QDomElement &element)
{
    QString name = element.attribute("Name");
    QString uid = element.attribute("uid");
    QString class_uid = element.attribute("class-uid");
    QString sourceUid = element.attribute("source-uid");
    QString locked = element.attribute("locked");
    QString wasLocked = element.attribute("was-locked");

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
                DiagramTableDescriptor* tabDesc = createDiagramTableDescriptor(doc, element.childNodes().at(i).childNodes().at(j).toElement(), v);
                result->addTableDescriptor(tabDesc);
            }
        }

        if(element.childNodes().at(i).nodeName() == "Notes")
        {
            for(int j=0; j<element.childNodes().at(i).childNodes().count(); j++)
            {
                DiagramNoteDescriptor* noteDesc = createDiagramNoteDescriptor(doc, element.childNodes().at(i).childNodes().at(j).toElement(), v);
                result->addNoteDescriptor(noteDesc);
            }
        }

        if(element.childNodes().at(i).nodeName() == "FKs")
        {
            for(int j=0; j<element.childNodes().at(i).childNodes().count(); j++)
            {
                DiagramFKDescriptor* fkDesc = createDiagramFKDescriptor(doc, element.childNodes().at(i).childNodes().at(j).toElement(), v);
                result->addFKDescriptor(fkDesc);
            }
        }
    }

    // means, there's a tree item for this diagram
    result->setSaved(true);
    if(sourceUid.length())
    {
        result->setSourceUid(QUuid(sourceUid));
    }
    else
    {
        result->setSourceUid(nullUid);
    }
    if(locked == "1")
    {
        result->lock(LockableElement::LOCKED);
    }
    else
    {
        result->unlock();
    }
    result->forceSetWasLocked(wasLocked == "1");
    return result;
}

/**
 * @brief DeserializationFactory::createTableInstance - creates a new table instance
 * @param v - the version
 * @param element - the XML element
 * @param secondStep - if this is the phase where the automatically "instantiated" table instances are being created or not
 * @return - the table instance
 */
TableInstance* DeserializationFactory::createTableInstance(Version* v, const QDomDocument&, const QDomElement &element, bool secondStep)
{
    QString name = element.attribute("Name");
    QString tabName = element.attribute("Table");
    bool refed = element.attribute("Ref") == "1";
    QString refTables = element.attribute("ReferencingTables");
    QString instantiatedTableInstances = element.attribute("InstantiatedTableInstances");
    QString uid = element.attribute("uid");
    QString class_uid = element.attribute("class-uid"); Q_UNUSED(class_uid);
    QString sourceUid = element.attribute("source-uid");
    QString locked = element.attribute("locked");
    QString wasLocked = element.attribute("was-locked");

    if(uid.length() == 0)
    {
        uid = QUuid::createUuid().toString();
    }

    QHash <QString, QVector<QString> > data;
    QMap<QString, QString> fkMapping;

    for(int i=0; i<element.childNodes().count(); i++)
    {
        if(element.childNodes().at(i).nodeName() == "Column")
        {
            QString colName = element.childNodes().at(i).toElement().attribute("Name");
            QVector<QString> col;
            for(int j=0; j<element.childNodes().at(i).toElement().childNodes().count(); j++)
            {
                col.append(element.childNodes().at(i).toElement().childNodes().at(j).toElement().attribute("value"));
            }
            data[colName] = col;
        }
        else
        if(element.childNodes().at(i).nodeName() == "FkMappings")
        {
            QDomElement el = element.childNodes().at(i).toElement();
            for(int j =0; j<el.childNodes().count(); j++)
            {
                QString fkName = el.childNodes().at(j).toElement().attribute("FK");
                QString tinstName = el.childNodes().at(j).toElement().attribute("TINST");
                fkMapping[fkName] = tinstName;
            }
        }
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
        Table* tbl = v->getTable(tabName);
        TableInstance* result = 0;
        if(tbl)
        {
            result = new TableInstance(tbl, refed, uid, v);
        }
        else
        {
            result = new TableInstance(name, refed, uid, v);
            result->forceSetTableName(tabName);
        }

        QStringList lst = refTables.split(QChar(','), QString::SkipEmptyParts);
        for(int i=0; i<lst.size(); i++)
        {
            Table* tab = v->getTable(lst.at(i));
            result->addTableReferencingThis(tab);
        }

        result->setValues(data);
        result->setName(name);

        if(sourceUid.length()) result->setSourceUid(sourceUid);
        else result->setSourceUid(nullUid);

        if(locked == "1") result->lock(LockableElement::LOCKED);
        else result->unlock();

        result->forceSetWasLocked(wasLocked == "1");
        result->setFkMappings(fkMapping);
        QVector<QString> failedFks;
        result->finalizeFkMappings(failedFks);
        // fetch the FK mappings
        return result;
    }
}

Procedure* DeserializationFactory::createProcedure(Version* v,  const QDomDocument&, const QDomElement& element)
{
    QString name = element.attribute("Name");
    QString uid = element.attribute("uid");
    QString class_uid = element.attribute("class-uid");
    QString sourceUid = element.attribute("source-uid");
    QString locked = element.attribute("locked");
    QString wasLocked = element.attribute("was-locked");
    QString guided = element.hasAttribute("guided")?element.attribute("guided"):"0";

    bool guidedCreation = guided =="1";
    if(uid.length() == 0)
    {
        uid = QUuid::createUuid().toString();
    }
    if(class_uid != uidProcedure)
    {

    }
    Procedure* p = new Procedure(name, uid, v, guidedCreation);

    if(guidedCreation)
    {
        populateGuidedParametersForStoredMethod(*p, element.firstChildElement("Parameters"));
        populateGuidedDescrptionForStoredMethod(*p, element.firstChildElement("Descriptions"));

        bool javaMap = element.attribute("java-mapped").toInt() == 1;
        if(javaMap)
        {
            QString javaClass = element.attribute("java-class");
            QString javaMethod = element.attribute("java-method");

            p->setJavaClassName(javaClass);
            p->setJavaMethodName(javaMethod);

            p->setJavaBinding(true);
        }
    }

    if(sourceUid.length()) p->setSourceUid(sourceUid);
    else p->setSourceUid(nullUid);

    if(locked == "1") p->lock(LockableElement::LOCKED);
    else p->unlock();

    p->forceSetWasLocked(wasLocked == "1");

    QDomElement sqlElement = element.firstChildElement("Sql");
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

void DeserializationFactory::populateGuidedParametersForStoredMethod(StoredMethod &mth, const QDomElement &element)
{
    int cnt = element.attribute("count").toInt();
    if(cnt == 0)
    {
        return;
    }
    QVector<ParameterAndDescription*> pads(cnt);
    for(int i=0; i<element.childNodes().size(); i++)
    {
        QDomElement elI = element.childNodes().at(i).toElement();
        QString paramName = elI.attribute("name");
        QString paramType = elI.attribute("type");
        QString paramDirection = elI.attribute("direction");
        QString paramprogLangType = elI.attribute("prog-lang-type");
        QString paramDescr = elI.attribute("description");
        int index = elI.attribute("index").toInt();

        ParameterAndDescription* pad = new ParameterAndDescription(paramName, paramType, paramDescr, paramDirection, paramprogLangType, GUIDED);
        pads[index] = pad;
    }
    mth.setParameters(pads);
}

void DeserializationFactory::populateGuidedDescrptionForStoredMethod(StoredMethod &mth, const QDomElement &element)
{
    mth.setBriefDescr(element.attribute("brief"));
    mth.setDescription(element.firstChildElement("Description").firstChild().toCDATASection().data());
}


Function* DeserializationFactory::createFunction(Version* v,  const QDomDocument&, const QDomElement& element)
{
    QString name = element.attribute("Name");
    QString uid = element.attribute("uid");
    QString class_uid = element.attribute("class-uid");
    QString sourceUid = element.attribute("source-uid");
    QString locked = element.attribute("locked");
    QString wasLocked = element.attribute("was-locked");
    QString guided = element.hasAttribute("guided")?element.attribute("guided"):"0";

    bool guidedCreation = guided =="1";

    if(uid.length() == 0)
    {
        uid = QUuid::createUuid().toString();
    }
    if(class_uid != uidFunction)
    {

    }
    Function* func = new Function(name, uid, v, guidedCreation);

    if(guidedCreation)
    {
        populateGuidedParametersForStoredMethod(*func, element.firstChildElement("Parameters"));
        populateGuidedDescrptionForStoredMethod(*func, element.firstChildElement("Descriptions"));
        QString returns = element.attribute("returns");
        func->setReturn(returns);
        func->setReturnDesc(element.firstChildElement("ReturnDescription").firstChild().toCDATASection().data());

        bool javaMap = element.attribute("java-mapped").toInt() == 1;
        if(javaMap)
        {
            QString javaReturns = element.attribute("java-returns");
            QString javaClass = element.attribute("java-class");
            QString javaMethod = element.attribute("java-method");

            func->setJavaClassName(javaClass);
            func->setJavaMethodName(javaMethod);
            func->setJavaReturnType(javaReturns);

            func->setJavaBinding(true);
        }
    }

    if(sourceUid.length()) func->setSourceUid(sourceUid);
    else func->setSourceUid(nullUid);

    if(locked == "1") func->lock(LockableElement::LOCKED);
    else func->unlock();

    func->forceSetWasLocked(wasLocked == "1");

    QDomElement sqlElement = element.firstChildElement("Sql");
    if(sqlElement.hasAttribute("Encoded"))
    {
        if(sqlElement.attribute("Encoded") == "Base64")
        {
            QDomCDATASection cdata = sqlElement.firstChild().toCDATASection();
            QByteArray encoded = QByteArray(cdata.toText().data().toLocal8Bit());
            func->setSql(QString(QByteArray::fromBase64(encoded)));

            return func;
        }
    }

    // assuming a not encoded SQL, old versions, pre 0.1i
    QDomCDATASection cdata = sqlElement.firstChild().toCDATASection();
    func->setSql(cdata.toText().data());

    return func;
}

Trigger* DeserializationFactory::createTrigger(Version* v,  const QDomDocument&, const QDomElement& element)
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

    Trigger* trigg = new Trigger(name, uid, v);
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

    QString wasLocked = element.attribute("was-locked");
    QString locked = element.attribute("locked");
    QString sourceUid = element.attribute("source-uid");
    if(locked == "1")
    {
        trigg->lock(LockableElement::LOCKED);
    }
    else
    {
        trigg->unlock();
    }

    if(sourceUid.length())
    {
        trigg->setSourceUid(sourceUid);
    }
    else
    {
        trigg->setSourceUid(nullUid);
    }
    trigg->forceSetWasLocked(wasLocked == "1");

    return trigg;
}

Patch* DeserializationFactory::createPatch(Project* /*p*/, Version* v, const QDomDocument& /*doc*/, const QDomElement& element)
{
    Patch* patch = new Patch(v, true);

    patch->setName(element.attribute("name"));
    patch->setSuspended(element.attribute("suspended") == "1");
    patch->setForcedUid(element.attribute("uid"));

    for(int i=0; i<element.childNodes().count(); i++)
    {
        // get the locked uids
        if(element.childNodes().at(i).nodeName() == "LockedUids")
        {
            QStringList uids;
            QDomElement el = element.childNodes().at(i).toElement();
            for(int j=0; j<el.childNodes().count(); j++)
            {
                QString uid = el.childNodes().at(j).toElement().attribute("value");
                QString classUid = el.childNodes().at(j).toElement().attribute("class-uid");
                uids.append(uid);
                patch->addObjUidToClassUidMapEntry(uid, classUid);
            }
            patch->setLockedUids(uids);
        }

        // get the deleted uids
        if(element.childNodes().at(i).nodeName() == "DeletedUids")
        {
            QStringList uids;
            QDomElement el = element.childNodes().at(i).toElement();
            for(int j=0; j<el.childNodes().count(); j++)
            {
                QString uid = el.childNodes().at(j).toElement().attribute("value");
                QString classUid = el.childNodes().at(j).toElement().attribute("class-uid");
                uids.append(uid);
                patch->addObjUidToClassUidMapEntry(uid, classUid);
            }
            patch->setDeletedUids(uids);
        }

        // get the new uids
        if(element.childNodes().at(i).nodeName() == "NewUids")
        {
            QStringList uids;
            QDomElement el = element.childNodes().at(i).toElement();
            for(int j=0; j<el.childNodes().count(); j++)
            {
                QString uid = el.childNodes().at(j).toElement().attribute("value");
                QString classUid = el.childNodes().at(j).toElement().attribute("class-uid");
                uids.append(uid);
                patch->addObjUidToClassUidMapEntry(uid, classUid);
            }
            patch->setNewUids(uids);
        }

        // create the originals map
        if(element.childNodes().at(i).nodeName() == "Originals")
        {
            QDomElement el = element.childNodes().at(i).toElement();
            QMap<QString, QString>  map;
            for(int j=0; j<el.childNodes().count(); j++)
            {
                QDomElement cel = el.childNodes().at(j).toElement();
                QString uid = cel.attribute("uid");
                QDomCDATASection cdata = cel.firstChild().toCDATASection();
                QByteArray encoded = QByteArray(cdata.toText().data().toLocal8Bit());
                map.insert(uid, encoded);
            }
            patch->setOriginalMap(map);
        }

        if(element.childNodes().at(i).nodeName() == "TableDeletions")
        {
            QDomElement el = element.childNodes().at(i).toElement();
            QMap<QString, QVector<QString> >  map;
            for(int j=0; j<el.childNodes().count(); j++)    // MainObject
            {
                QString uid = el.childNodes().at(j).toElement().attribute("uid");
                QVector<QString> tabinstUids;
//                qDebug() << el.childNodes().at(j).nodeName() << " uid=" <<  uid;
                for(int k=0; k<el.childNodes().at(j).childNodes().count(); k++)
                {
                    QDomElement elPulledIn = el.childNodes().at(j).childNodes().at(k).toElement();
//                    qDebug() << elPulledIn.nodeName() << " uid=" << elPulledIn.attribute("uid");
                    tabinstUids.append(elPulledIn.attribute("uid"));
                }
                map[uid] = tabinstUids;
            }
            patch->setTempTabInstUidVector(map);
        }
    }

    return patch;
}


ObjectWithUid* DeserializationFactory::createElementForClassUid(const QString& classUid, const QString& serialized, Version *v)
{
    QString err;
    QDomDocument a("PatchData");
    if(!a.setContent(serialized, &err))
    {
//        qDebug() << "Cannot set a b64 encoded stuff:" << err;
        return 0;
    }
    QString node = a.documentElement().nodeName();
    if(node != "OriginalElement")
    {
//        qDebug() << "This is not an original element stuff but: " << node;
        return 0;
    }

    // now find the type based on the class-uid and replace the element with the original that was there
    QString class_uid = a.documentElement().firstChild().toElement().attribute("class-uid");
    if(classUid != class_uid)
    {
//        qDebug() << "This is not an expected (" << classUid << ") element but " << class_uid;
        return 0;

    }
    if(classUid == uidTable)
    {
        Table* tab = DeserializationFactory::createTable(v->getProject()->getEngine(), v, a, a.documentElement().firstChild().toElement());
        v->setupForeignKeyRelationshipsForATable(tab);
        return tab;
    }
    if(classUid == uidTableInstance)
    {
        // Warning! This does not set the table for the table instance! Need to set it later
        TableInstance* tabInst = DeserializationFactory::createTableInstance(v, a, a.documentElement().firstChild().toElement());
        return tabInst;
    }
    if(classUid.toUpper() == uidDiagram.toUpper())
    {
        Diagram* dgram = DeserializationFactory::createDiagram(v, a, a.documentElement().firstChild().toElement());
        return dgram;
    }
    if(classUid.toUpper() == uidView.toUpper())
    {
        View* view = DeserializationFactory::createView(v, v->getProject()->getEngine(), a, a.documentElement().firstChild().toElement());
        return view;
    }
    if(classUid.toUpper() == uidTrigger.toUpper())
    {
        Trigger* trg= DeserializationFactory::createTrigger(v, a, a.documentElement().firstChild().toElement());
        return trg;
    }
    if(classUid.toUpper() == uidProcedure.toUpper())
    {
        Procedure* proc= DeserializationFactory::createProcedure(v, a, a.documentElement().firstChild().toElement());
        return proc;
    }
    if(classUid.toUpper() == uidFunction.toUpper())
    {
        Function* func= DeserializationFactory::createFunction(v, a, a.documentElement().firstChild().toElement());
        return func;
    }
    if(classUid.toUpper() == uidNumericDT.toUpper()
        || classUid.toUpper() == uidStringDT.toUpper()
        || classUid.toUpper() == uidDateTimeDT.toUpper()
        || classUid.toUpper() == uidBooleanDT.toUpper()
        || classUid.toUpper() == uidBlobDT.toUpper()
        || classUid.toUpper() == uidMiscDT.toUpper()
        || classUid.toUpper() == uidSpatialDT.toUpper())
    {
        UserDataType* udt= DeserializationFactory::createUserDataType(v, a, a.documentElement().firstChild().toElement());
        return udt;
    }

    return 0;

}
