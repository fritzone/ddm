#include "StartupValuesHelper.h"

#include "core_Table.h"
#include "core_Column.h"
#include "core_UserDataType.h"

#include <QFileDialog>
#include <QTextStream>

void loadStartupValuesFromCSVIntoTable(QTableWidget *table, QWidget* parent)
{
    QString fileName = QFileDialog::getOpenFileName(parent,  "Load values", "", "CSV files (*.csv)");
    if(fileName.length() == 0)
    {
        return;
    }

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    table->clearContents();
    table->setRowCount(0);

    int i = 0;
    QTextStream in(&file);
    while (!in.atEnd())
    {
        table->insertRow(i);
        QString line = in.readLine();
        int j = 0, cc = 0;
        QString cellV = "";
        while(j<line.length())
        {
            if(line.at(j) !=',')
            {
                cellV += line.at(j);
            }
            else
            {
                QTableWidgetItem *twl = new QTableWidgetItem(cellV);
                table->setItem(i, cc, twl);
                cc ++;
                cellV = "";
            }
            j++;
        }
        // last value
        QTableWidgetItem *twl = new QTableWidgetItem(cellV);
        table->setItem(i, cc, twl);
        i++;
    }

}

void saveStartupValuesToCSVFromTable(QTableWidget* table, QWidget* parent)
{
    QVector<QString> rows;

    for(int i=0; i<table->rowCount(); i++)
    {
        QString rowI;
        for(int j=0; j<table->columnCount(); j++)
        {
            if(table->item(i,j))
            {
                rowI += table->item(i,j)->text();
            }
            else
            {
                rowI += "";
            }

            if(j<table->columnCount() - 1)
            {
                rowI += ",";
            }
        }
        rows.append(rowI);
    }
    QString fileName = QFileDialog::getSaveFileName(parent,  "Save values", "", "CSV files (*.csv)");
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }
    QTextStream out(&file);
    for(int i=0; i<rows.size(); i++)
    {
        out << rows[i] << "\n";
    }
}

void addNewRowToTable(QTableWidget* guiTable, Table* src)
{
    int curRowC = guiTable->rowCount();
    guiTable->insertRow(curRowC);
    for(int i=0; i<src->getColumns().count(); i++)
    {
        QWidget *defWidget = src->getColumns()[i]->getDataType()->getDefaultsTableWidget();
        if(defWidget != 0)
        {
            guiTable->setCellWidget(curRowC, i, defWidget);
        }
    }
}
