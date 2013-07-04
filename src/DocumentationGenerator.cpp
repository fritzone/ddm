#include "DocumentationGenerator.h"

#include "qhtml.h"
#include "Solution.h"
#include "Project.h"
#include "Version.h"
#include "core_Table.h"
#include "core_Column.h"
#include "core_UserDataType.h"
#include "core_Index.h"
#include "core_Procedure.h"
#include "core_Function.h"
#include "core_Trigger.h"
#include <QApplication>

DocumentationGenerator::DocumentationGenerator(const Solution* s, const QHtmlCSSStyleSet *ss) : m_solution(s), m_styles(ss)
{
}

QString DocumentationGenerator::getDocumentation()
{
    QString result;
    QHtmlDocument doc(m_solution->getName(), QHtmlDocument::HTML_5, m_styles);

    QHtmlHeading* h1 = new QHtmlHeading(1);
    QHtmlText* solutionName = new QHtmlText(QApplication::tr("Solution: ") + m_solution->getName());
    h1->addElement(solutionName);
    doc.getBody()->addHeading(h1);

    QHtmlHeading* h2 = new QHtmlHeading(2);
    QHtmlText* projectName = new QHtmlText(QApplication::tr("Project: ") + m_solution->currentProject()->getName());
    h2->addElement(projectName);
    doc.getBody()->addHeading(h2);

    QHtmlText* projectDescription = new QHtmlText(m_solution->currentProject()->getDescription());
    doc.getBody()->addElement(projectDescription);

    QHtmlHeading* h3Tables = new QHtmlHeading(3);
    QHtmlText* tablesText = new QHtmlText(QApplication::tr("Tables"));
    h3Tables->addElement(tablesText);
    doc.getBody()->addHeading(h3Tables);

    QVector<Table*> tabs = m_solution->currentProject()->getWorkingVersion()->getTables();
    for(int i=0; i<tabs.size(); i++)
    {
        Table* tabI = tabs.at(i);
        getDocumentationForTable(tabI, doc);
    }

    // Procedures and functions
    generateHtmlForStoredMethods((QVector<StoredMethod*>*)(&m_solution->currentProject()->getWorkingVersion()->getProcedures()), QApplication::tr("Procedures"), doc);
    generateHtmlForStoredMethods((QVector<StoredMethod*>*)(&m_solution->currentProject()->getWorkingVersion()->getFunctions()), QApplication::tr("Functions"), doc);

    // Triggers
    {
    QHtmlHeading* h3TableTriggers = new QHtmlHeading(3);
    QHtmlText* triggerText= new QHtmlText(QApplication::tr("Triggers"));
    h3TableTriggers->addElement(triggerText);
    doc.getBody()->addHeading(h3TableTriggers);
    QVector<Trigger*> triggers = m_solution->currentProject()->getWorkingVersion()->getTriggers();
    for(int i=0; i<triggers.size(); i++)
    {
        getDocumentationForTrigger(triggers.at(i), doc);
    }
    }

    result = doc.html();
    return result;
}


void DocumentationGenerator::getDocumentationForTrigger(Trigger *trg, QHtmlDocument &doc)
{
    QHtmlHeading* h4Trg = new QHtmlHeading(4);
    QHtmlText* trigName= new QHtmlText(trg->getName());
    h4Trg->addElement(trigName);
    doc.getBody()->addHeading(h4Trg);

    QHtmlText* trigDesc = new QHtmlText(trg->getDescription(), QHtmlCSSClass::classDescription());
    doc.getBody()->addElement(trigDesc);
    doc.getBody()->addElement(new QHtmlBreak());

    // TODO: tis down here is ugly
    QHtmlText* trigTable = new QHtmlText("Table: <b>" + trg->getTable() + "</b>", QHtmlCSSClass::classDescription());
    doc.getBody()->addElement(trigTable);
    doc.getBody()->addElement(new QHtmlBreak());
    QHtmlText* trigTime = new QHtmlText("Timing: <b>" + trg->getTime() + "</b>", QHtmlCSSClass::classDescription());
    doc.getBody()->addElement(trigTime);
    doc.getBody()->addElement(new QHtmlBreak());
    QHtmlText* trigMethod = new QHtmlText("Event: <b>" + trg->getEvent() + "</b>", QHtmlCSSClass::classDescription());
    doc.getBody()->addElement(trigMethod);
    doc.getBody()->addElement(new QHtmlBreak());


}

void DocumentationGenerator::getDocumentationForTable(const Table* table, QHtmlDocument& doc)
{
    QHtmlHeading* h4Table = new QHtmlHeading(4);
    QHtmlText* tableName= new QHtmlText(table->getName());
    h4Table->addElement(tableName);
    doc.getBody()->addHeading(h4Table);

    QHtmlText* tableDesc = new QHtmlText(table->getDescription(), QHtmlCSSClass::classDescription());
    doc.getBody()->addElement(tableDesc);
    doc.getBody()->addElement(new QHtmlBreak());
    // Columns
    {
    QHtmlHeading* h5TableColumns = new QHtmlHeading(4);
    QHtmlText* tableColumnsText= new QHtmlText(QApplication::tr("Columns"));
    h5TableColumns->addElement(tableColumnsText);
    doc.getBody()->addHeading(h5TableColumns);

    QHtmlTable* tableForColumns = new QHtmlTable(1);
    doc.getBody()->addElement(tableForColumns);
    doc.getBody()->addElement(new QHtmlBreak());

    QHtmlText* colTabHeaderTextColPRI = new QHtmlText(QApplication::tr("PRI"), QHtmlCSSClass::classTableHeader());
    QHtmlText* colTabHeaderTextColName = new QHtmlText(QApplication::tr("Name"), QHtmlCSSClass::classTableHeader());
    QHtmlText* colTabHeaderTextColType = new QHtmlText(QApplication::tr("SQL Type"), QHtmlCSSClass::classTableHeader());
    QHtmlText* colTabHeaderTextColDesc = new QHtmlText(QApplication::tr("Description"), QHtmlCSSClass::classTableHeader());

    QHtmlTableHeader* hdrColTabHeaderTextColPRI = new QHtmlTableHeader(colTabHeaderTextColPRI);
    QHtmlTableHeader* hdrColTabHeaderTextColName = new QHtmlTableHeader(colTabHeaderTextColName);
    QHtmlTableHeader* hdrColTabHeaderTextColType = new QHtmlTableHeader(colTabHeaderTextColType);
    QHtmlTableHeader* hdrColTabHeaderTextColDesc = new QHtmlTableHeader(colTabHeaderTextColDesc);

    QHtmlRow* hdrColRow = new QHtmlRow();
    hdrColRow->addData(hdrColTabHeaderTextColPRI);
    hdrColRow->addData(hdrColTabHeaderTextColName);
    hdrColRow->addData(hdrColTabHeaderTextColType);
    hdrColRow->addData(hdrColTabHeaderTextColDesc);
    tableForColumns->addRow(hdrColRow);

    QStringList cols = table->fullColumns();
    for(int j=0; j<cols.size(); j++)
    {
        Column* c = table->getColumn(cols.at(j));
        if(c == 0) c = table->getColumnFromParents(cols.at(j));
        if(c == 0) continue;

        QHtmlRow* rowJ = new QHtmlRow();

        QHtmlText* colPri = new QHtmlText(c->isPk()?QApplication::tr("Y"):"", QHtmlCSSClass::classTableText());
        QHtmlRowData* rowJdataColPri = new QHtmlRowData(colPri);
        rowJ->addData(rowJdataColPri);

        QHtmlText* colName = new QHtmlText(cols.at(j), QHtmlCSSClass::classTableText());
        QHtmlRowData* rowJdataColName = new QHtmlRowData(colName);
        rowJ->addData(rowJdataColName);

        QHtmlText* colType = new QHtmlText(c->getDataType()->sqlAsString(), QHtmlCSSClass::classCode());
        QHtmlRowData* rowJdataColType = new QHtmlRowData(colType);
        rowJ->addData(rowJdataColType);

        QHtmlText* colDesc = new QHtmlText(c->getDescription(), QHtmlCSSClass::classTableText());
        QHtmlRowData* rowJdataColDesc = new QHtmlRowData(colDesc);
        rowJ->addData(rowJdataColDesc);

        tableForColumns->addRow(rowJ);
    }
    }

    // Indices
    {
    QStringList idxs = table->fullIndices();
    if(idxs.size() > 0)
    {
        QHtmlHeading* h5TableIndices = new QHtmlHeading(4);
        QHtmlText* tableColumnsText= new QHtmlText(QApplication::tr("Indices"));
        h5TableIndices->addElement(tableColumnsText);
        doc.getBody()->addHeading(h5TableIndices);

        for(int j=0; j<idxs.size(); j++)
        {
            Index* idx = table->getIndex(idxs.at(j));
            if(idx == 0) idx = table->getIndexFromParents(idxs.at(j));
            if(idx == 0) continue;

            QHtmlHeading* h6IndexName = new QHtmlHeading(5);
            QHtmlText* tableIndexNameText= new QHtmlText(idxs.at(j));
            h6IndexName->addElement(tableIndexNameText);
            doc.getBody()->addHeading(h6IndexName);

            QHtmlTable* tableForIndexes = new QHtmlTable(1);
            QHtmlText* colTabHeaderTextIdxName = new QHtmlText(QApplication::tr("Name"), QHtmlCSSClass::classTableHeader());
            QHtmlText* colTabHeaderTextIdxOrder = new QHtmlText(QApplication::tr("Order"), QHtmlCSSClass::classTableHeader());

            QHtmlTableHeader* hdrColTabHeaderTextIdxName = new QHtmlTableHeader(colTabHeaderTextIdxName);
            QHtmlTableHeader* hdrColTabHeaderTextIdxOrder= new QHtmlTableHeader(colTabHeaderTextIdxOrder);

            QHtmlRow* hdrColRow = new QHtmlRow();
            hdrColRow->addData(hdrColTabHeaderTextIdxName);
            hdrColRow->addData(hdrColTabHeaderTextIdxOrder);

            tableForIndexes->addRow(hdrColRow);
            for(int k = 0; k<idx->getColumns().size(); k++)
            {
                QHtmlRow* rowK = new QHtmlRow();
                QHtmlText* idxN = new QHtmlText(idx->getColumns().at(k)->getName(), QHtmlCSSClass::classTableText());
                QHtmlText* idxO = new QHtmlText(idx->getOrderForColumn(idx->getColumns().at(k)), QHtmlCSSClass::classTableText());
                QHtmlRowData* dataN = new QHtmlRowData(idxN);
                QHtmlRowData* dataO = new QHtmlRowData(idxO);
                rowK->addData(dataN);
                rowK->addData(dataO);
                tableForIndexes->addRow(rowK);
            }

            doc.getBody()->addElement(tableForIndexes);
        }
    }
    }

}

void DocumentationGenerator::getDocumentationForStoredMethod(StoredMethod *mth, QHtmlDocument &doc)
{
    QHtmlHeading* h4Proc = new QHtmlHeading(4);
    QHtmlText* procName= new QHtmlText(mth->getName());
    h4Proc->addElement(procName);
    doc.getBody()->addHeading(h4Proc);

    QVector<ParameterAndDescription> pads = mth->getParametersWithDescription();
    QHtmlText* methodBriefDescription = new QHtmlText(mth->getBriefDescription(), mth->getBriefDescription().indexOf("TODO")>-1?
                                                          QHtmlCSSClass::classTODO():QHtmlCSSClass::classDescription());
    doc.getBody()->addElement(methodBriefDescription);

    if(dynamic_cast<Function*>(mth))
    {
        doc.getBody()->addElement(new QHtmlBreak());
        QHtmlText* funcReturn = new QHtmlText(QApplication::tr("Returns: ") + mth->getReturns(),
                                              mth->getReturns().indexOf("TODO")>-1?
                                                  QHtmlCSSClass::classTODO():QHtmlCSSClass::classDescription());
        doc.getBody()->addElement(funcReturn);
    }

    QHtmlTable* tableForParameterTypes = new QHtmlTable(1);
    QHtmlText* colTabHeaderTextParName = new QHtmlText(QApplication::tr("Name"), QHtmlCSSClass::classTableHeader());
    QHtmlText* colTabHeaderTextParType = new QHtmlText(QApplication::tr("Type"), QHtmlCSSClass::classTableHeader());
    QHtmlText* colTabHeaderTextParDesc = new QHtmlText(QApplication::tr("Description"), QHtmlCSSClass::classTableHeader());

    QHtmlTableHeader* hdrMethTabHeaderTextParName = new QHtmlTableHeader(colTabHeaderTextParName);
    QHtmlTableHeader* hdrMethTabHeaderTextParType = new QHtmlTableHeader(colTabHeaderTextParType);
    QHtmlTableHeader* hdrMethTabHeaderTextParDesc = new QHtmlTableHeader(colTabHeaderTextParDesc);

    QHtmlRow* hdrColRow = new QHtmlRow();
    hdrColRow->addData(hdrMethTabHeaderTextParName);
    hdrColRow->addData(hdrMethTabHeaderTextParType);
    hdrColRow->addData(hdrMethTabHeaderTextParDesc);
    tableForParameterTypes->addRow(hdrColRow);
    bool needs_reminder = false;
    bool at_least_1_parameter_added = false;
    for(int j=0; j<pads.size(); j++)
    {
        if(pads.at(j).m_source > 0)
        {
            QHtmlText* colParName = new QHtmlText(pads.at(j).m_parameter, QHtmlCSSClass::classTableText());
            QHtmlText* colParType = new QHtmlText(pads.at(j).m_type, QHtmlCSSClass::classTableText());
            QHtmlText* colParDesc = new QHtmlText(pads.at(j).m_description,
                                                  pads.at(j).m_description.indexOf("TODO")>-1?
                                                      QHtmlCSSClass::classTODO():QHtmlCSSClass::classTableText());

            QHtmlRowData* dataN = new QHtmlRowData(colParName);
            QHtmlRowData* dataO = new QHtmlRowData(colParType);
            QHtmlRowData* dataD = new QHtmlRowData(colParDesc);
            QHtmlRow* rowK = new QHtmlRow();

            rowK->addData(dataN);
            rowK->addData(dataO);
            rowK->addData(dataD);
            tableForParameterTypes->addRow(rowK);

            at_least_1_parameter_added = true;
        }
        else
        {
            needs_reminder = true;
        }
    }

    if(at_least_1_parameter_added)
    {
        doc.getBody()->addElement(new QHtmlBreak());
        doc.getBody()->addElement(tableForParameterTypes);
        doc.getBody()->addElement(new QHtmlBreak());
    }

    if(needs_reminder)
    {
        QHtmlText* txtTODO = new QHtmlText(QString("TODO: ") + QApplication::tr("Update the documentation for the method"), QHtmlCSSClass::classTODO());
        doc.getBody()->addElement(txtTODO);
        doc.getBody()->addElement(new QHtmlBreak());
    }

    QHtmlText* txtDesc = new QHtmlText(mth->getDescription(), QHtmlCSSClass::classDescription());
    doc.getBody()->addElement(txtDesc);
}


void DocumentationGenerator::generateHtmlForStoredMethods(QVector<StoredMethod*>* _methods, const QString& header, QHtmlDocument& doc)
{
    QVector<StoredMethod*> methods = *_methods;
    QHtmlHeading* h3TableProcedures = new QHtmlHeading(3);
    QHtmlText* tableColumnsText= new QHtmlText(header);
    h3TableProcedures->addElement(tableColumnsText);
    doc.getBody()->addHeading(h3TableProcedures);

    for(int i=0; i<methods.size(); i++)
    {
        getDocumentationForStoredMethod(methods.at(i), doc);
    }

}
