#include "DocumentationGenerator.h"

#include "qhtml.h"
#include "Solution.h"
#include "Project.h"
#include "Version.h"
#include "core_Table.h"
#include "core_Column.h"
#include "core_UserDataType.h"

DocumentationGenerator::DocumentationGenerator(const Solution* s) : m_solution(s)
{
}

QString DocumentationGenerator::getDocumentation()
{
    QString result;
    QHtmlDocument doc(m_solution->getName());

    QHtmlHeading* h1 = new QHtmlHeading(1);
    QHtmlText* solutionName = new QHtmlText(QString("Solution: ") + m_solution->getName());
    h1->addElement(solutionName);
    doc.getBody()->addHeading(h1);

    QHtmlHeading* h2 = new QHtmlHeading(2);
    QHtmlText* projectName = new QHtmlText(QString("Project: ") + m_solution->currentProject()->getName());
    h2->addElement(projectName);
    doc.getBody()->addHeading(h2);

    QHtmlText* projectDescription = new QHtmlText(m_solution->currentProject()->getDescription());
    doc.getBody()->addElement(projectDescription);

    QHtmlHeading* h3Tables = new QHtmlHeading(3);
    QHtmlText* tablesText = new QHtmlText(QString("Tables"));
    h3Tables->addElement(tablesText);
    doc.getBody()->addHeading(h3Tables);

    QVector<Table*> tabs = m_solution->currentProject()->getWorkingVersion()->getTables();
    for(int i=0; i<tabs.size(); i++)
    {
        QHtmlHeading* h4Table = new QHtmlHeading(4);
        QHtmlText* tableName= new QHtmlText(tabs.at(i)->getName());
        h4Table->addElement(tableName);
        doc.getBody()->addHeading(h4Table);

        QHtmlText* tableDesc = new QHtmlText(tabs.at(i)->getDescription());
        doc.getBody()->addElement(tableDesc);

        QHtmlHeading* h5TableColumns = new QHtmlHeading(5);
        QHtmlText* tableColumnsText= new QHtmlText("Columns");
        h5TableColumns->addElement(tableColumnsText);
        doc.getBody()->addHeading(h5TableColumns);

        QHtmlTable* tableForColumns = new QHtmlTable(1);
        doc.getBody()->addElement(tableForColumns);

        QHtmlText* colTabHeaderTextColPRI = new QHtmlText("PRI");
        QHtmlText* colTabHeaderTextColName = new QHtmlText("Name");
        QHtmlText* colTabHeaderTextColType = new QHtmlText("SQL Type");
        QHtmlText* colTabHeaderTextColDesc = new QHtmlText("Description");

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

        QStringList cols = tabs.at(i)->fullColumns();
        for(int j=0; j<cols.size(); j++)
        {
            Column* c = tabs.at(i)->getColumn(cols.at(j));
            if(c == 0) c = tabs.at(i)->getColumnFromParents(cols.at(j));
            if(c == 0) continue;

            QHtmlRow* rowJ = new QHtmlRow();

            QHtmlText* colPri = new QHtmlText(c->isPk()?"Y":"");
            QHtmlRowData* rowJdataColPri = new QHtmlRowData(colPri);
            rowJ->addData(rowJdataColPri);

            QHtmlText* colName = new QHtmlText(cols.at(j));
            QHtmlRowData* rowJdataColName = new QHtmlRowData(colName);
            rowJ->addData(rowJdataColName);

            QHtmlText* colType = new QHtmlText(c->getDataType()->sqlAsString());
            QHtmlRowData* rowJdataColType = new QHtmlRowData(colType);
            rowJ->addData(rowJdataColType);

            QHtmlText* colDesc = new QHtmlText(c->getDescription());
            QHtmlRowData* rowJdataColDesc = new QHtmlRowData(colDesc);
            rowJ->addData(rowJdataColDesc);

            tableForColumns->addRow(rowJ);
        }
    }

    result = doc.html();
    return result;
}
