#ifndef TABLECOMPARISONFORM_H
#define TABLECOMPARISONFORM_H

#include <QWidget>
#include <QSplitter>
#include <QMap>
#include <QTreeWidgetItem>

class Table;
class Column;
class Version;
class SqlHighlighter;
class DatabaseEngine;

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

    enum Mode
    {
        COMPARE_TABLES,
        COMPARE_VERSIONS
    };

    explicit TableComparisonForm(Mode, QWidget *parent = 0);
    ~TableComparisonForm();

    void setLeftTable(Table *t);
    void setRightTable(Table *t);
    
    void populateTree();

    void setFromVersion(Version* v);
    void setToVersion(Version* v);

    void setSqlList(QStringList s)
    {
        sqlList = s;
    }

private:

    void populateColumns();

private slots:

    void leftItemSelected(const QString&);
    void rightItemSelected(const QString&);
    void onSave();
    void onInject();

private:

    void setupNewTable(const QString& v, Table*& table);
    QString addCommands(const QStringList&);

private:
    Ui::TableComparisonForm *ui;
    Table* m_leftTable;
    Table* m_rightTable;
    QSplitter* m_spl;
    QMap<Sides, QTreeWidget*> m_trees;
    SqlHighlighter* highlighter;
    Mode m_mode;
    Version* m_from;
    Version* m_to;
    QStringList sqlList;
    DatabaseEngine* m_engine;

};

#endif // TABLECOMPARISONFORM_H
