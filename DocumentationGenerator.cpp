#include "DocumentationGenerator.h"

#include "qhtml.h"
#include "Solution.h"
#include "Project.h"
#include "Version.h"
#include "core_Table.h"
#include "core_Column.h"

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

        QHtmlTable* tableForColumns = new QHtmlTable();
        doc.getBody()->addElement(tableForColumns);

        QStringList cols = tabs.at(i)->fullColumns();
        for(int j=0; j<cols.size(); j++)
        {
            QHtmlRow* rowJ = new QHtmlRow();
            rowJ->addRowData(cols.at(j));
            tableForColumns->addRow(rowJ);
        }
    }

    result = doc.html();
    return result;
}
