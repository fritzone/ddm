#ifndef TABLECOMPARISONFORM_H
#define TABLECOMPARISONFORM_H

#include <QWidget>
#include <QSplitter>
#include <QMap>
#include <QTreeWidgetItem>
class Table;

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

    void setLeftTable(Table *t)
    {
        m_leftTable = t;
    }

    void setRightTable(Table *t)
    {
        m_leftTable = t;
    }
    
    void populateTree();

private:
    Ui::TableComparisonForm *ui;
    Table* m_leftTable;
    Table* m_rightTable;
    QSplitter* m_spl;
    QMap<Sides, QTreeWidgetItem*> m_mainItems;
};

#endif // TABLECOMPARISONFORM_H
