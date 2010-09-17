#ifndef DATATYPESLISTFORM_H
#define DATATYPESLISTFORM_H

#include <QtGui/QWidget>
#include <QTreeWidgetItem>

class MainWindow;

namespace Ui
{
    class DataTypesListForm;
}

class UserDataType;

class DataTypesListForm : public QWidget
{
    Q_OBJECT
public:
    DataTypesListForm(QWidget *parent = 0);
    ~DataTypesListForm();
    void feedInDataTypes(const QVector<UserDataType*>& dataTypes);

public slots:

    void doubleClickTree(QTreeWidgetItem* item, int);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DataTypesListForm *m_ui;
    MainWindow* mw;
};

#endif // DATATYPESLISTFORM_H
