#ifndef FOREIGNKEYRECCOMENDATIONISSUE_H
#define FOREIGNKEYRECCOMENDATIONISSUE_H

#include "Issue.h"

class Table;
class Column;

class ForeignKeyReccomendationIssue : public Issue
{
public:
    ForeignKeyReccomendationIssue(Table* newTable, Column* newColumn, Table* firstTable, Column* firstColumn);

    virtual bool stillValid();
    virtual bool isLike(Issue *);
    Table* getNewTable() const { return m_newTable; }
    Table* getFirstTable() const { return m_firstTable; }
    Column* getNewColumn() const { return m_newColumn; }
    Column* getFirstColumn() const { return m_firstColumn; }

    QString getNewTableName() const { return m_newTableName; }
    QString getFirstTableName() const { return m_firstTableName; }
    QString getNewColumnName() const { return m_newColumnName; }
    QString getFirstColumnName() const { return m_firstColumnName; }

private:
    Table* m_newTable;
    Table* m_firstTable;
    Column* m_newColumn;
    Column* m_firstColumn;
    QString m_newTableName;
    QString m_firstTableName;
    QString m_newColumnName;
    QString m_firstColumnName;
};

#endif // FOREIGNKEYRECCOMENDATIONISSUE_H
