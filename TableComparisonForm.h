#ifndef TABLECOMPARISONFORM_H
#define TABLECOMPARISONFORM_H

#include <QWidget>
#include <QSplitter>
#include <QMap>
#include <QTreeWidgetItem>

class Table;
class Column;
class SqlHighlighter;

namespace Ui {
class TableComparisonForm;
}

class TableComparisonForm : public QWidget
{
    Q_OBJECT
    
    enum Sides
    {
        LEFT, RIGHT
    };

public:
    explicit TableComparisonForm(QWidget *parent = 0);
    ~TableComparisonForm();

    void setLeftTable(Table *t);
    void setRightTable(Table *t);
    
    void populateTree();

private:

    void populateColumns();

private slots:

    void leftItemSelected(const QString&);
    void rightItemSelected(const QString&);

private:

    void populateClosestMatchingColumn(Column* left, QTreeWidgetItem *colItemLeft, QTreeWidgetItem *colItemRight);

private:
    Ui::TableComparisonForm *ui;
    Table* m_leftTable;
    Table* m_rightTable;
    QSplitter* m_spl;
    QMap<Sides, QTreeWidget*> m_trees;
    SqlHighlighter* highlighter;
};

#endif // TABLECOMPARISONFORM_H
