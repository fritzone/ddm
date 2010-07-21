#ifndef STARTUPVALUESHELPER_H
#define STARTUPVALUESHELPER_H

#include <QTableWidget>

class Table;

void loadStartupValuesFromCSVIntoTable(QTableWidget* table, QWidget* parent);

void saveStartupValuesToCSVFromTable(QTableWidget* table, QWidget* parent);

void addNewRowToTable(QTableWidget* guiTable, Table* src);

#endif // STARTUPVALUESHELPER_H
