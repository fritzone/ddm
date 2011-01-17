#include "ForeignKeyReccomendationIssue.h"
#include "IssueManager.h"
#include "Table.h"
#include "Column.h"
#include "UserDataType.h"
#include "Workspace.h"
#include "Version.h"

#include <QApplication>

ForeignKeyReccomendationIssue::ForeignKeyReccomendationIssue(Table* newTable, Column* newColumn, Table* firstTable, Column* firstColumn):
    Issue(firstColumn, QApplication::tr("This column might need a foreign key to table: ")
        + newTable->getName()
        + QApplication::tr(" column: ")
        + newColumn->getName(),
        Issue::RECOMMENDATION, IssueManager::getInstance().nextCounter()),

    m_newTable(newTable), m_firstTable(firstTable), m_newColumn(newColumn), m_firstColumn(firstColumn)
{
}


bool ForeignKeyReccomendationIssue::stillValid()
{
    if(!Workspace::getInstance()->workingVersion()->hasTable(m_firstTableName)) return false;
    if(!Workspace::getInstance()->workingVersion()->hasTable(m_newTableName)) return false;
    Table* firstTable = Workspace::getInstance()->workingVersion()->getTable(m_firstTableName);
    if(!firstTable->hasColumn(m_firstColumnName)) return false; // column name was changed. issue will be removed, then recreated
    Table* newTable = Workspace::getInstance()->workingVersion()->getTable(m_newTableName);
    if(!newTable->hasColumn(m_newColumnName)) return false; // column name was changed
    return m_firstColumn->getDataType()->getName() == m_newColumn->getDataType()->getName()
            && m_newColumn->getDataType()->getType() == DataType::DT_NUMERIC;
}

bool ForeignKeyReccomendationIssue::isLike(Issue* other)
{
    ForeignKeyReccomendationIssue* oth = dynamic_cast<ForeignKeyReccomendationIssue*>(other);
    if(!oth) return false;
    if(!stillValid()) return false;
    if(m_newColumn == oth->getNewColumn() && m_newTable == oth->getNewTable()
        && m_firstColumn == oth->getFirstColumn() && m_firstTable == oth->getFirstTable())
        return true;
    return false;
}

bool ForeignKeyReccomendationIssue::affects(Table *tab)
{
    return m_firstTable->getName() == tab->getName() || m_newTable->getName() == tab->getName();
}
