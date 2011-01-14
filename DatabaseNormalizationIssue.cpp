#include "DatabaseNormalizationIssue.h"
#include "IssueManager.h"
#include "Table.h"
#include "Column.h"
#include "UserDataType.h"
#include "Workspace.h"
#include "Version.h"

#include <QApplication>

DatabaseNormalizationIssue::DatabaseNormalizationIssue(Table* newTable, Column* newColumn, Table* firstTable, Column* firstColumn):
    Issue(firstColumn, QApplication::tr("This column might destroy the normalization of the database in relation with table: ")
        + newTable->getName()
        + QApplication::tr(" column: ")
        + newColumn->getName(),
        Issue::WARNING, IssueManager::getInstance().nextCounter()),
    m_newTable(newTable), m_firstTable(firstTable), m_newColumn(newColumn), m_firstColumn(firstColumn),
    m_newTableName(newTable->getName()), m_firstTableName(firstTable->getName()),
    m_newColumnName(newColumn->getName()), m_firstColumnName(firstColumn->getName())
{
}

bool DatabaseNormalizationIssue::stillValid()
{
    if(!Workspace::getInstance()->workingVersion()->hasTable(m_firstTableName)) return false;
    if(!Workspace::getInstance()->workingVersion()->hasTable(m_newTableName)) return false;
    Table* firstTable = Workspace::getInstance()->workingVersion()->getTable(m_firstTableName);
    if(!firstTable->hasColumn(m_firstColumnName)) return false; // column name was changed. issue will be removed, then recreated
    Table* newTable = Workspace::getInstance()->workingVersion()->getTable(m_newTableName);
    if(!newTable->hasColumn(m_newColumnName)) return false; // column name was changed
    return m_firstColumn->getDataType()->getName() == m_newColumn->getDataType()->getName();
}

bool DatabaseNormalizationIssue::isLike(Issue *other)
{
    DatabaseNormalizationIssue* oth = dynamic_cast<DatabaseNormalizationIssue*>(other);
    if(!oth) return false;
    if(!stillValid()) return false;
    if(m_newColumn == oth->getNewColumn() && m_newTable == oth->getNewTable()
        && m_firstColumn == oth->getFirstColumn() && m_firstTable == oth->getFirstTable())
        return true;
    return false;
}
