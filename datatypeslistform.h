#ifndef DATATYPESLISTFORM_H
#define DATATYPESLISTFORM_H

#include <QtGui/QWidget>

namespace Ui {
    class DataTypesListForm;
}

class UserDataType;

class DataTypesListForm : public QWidget {
    Q_OBJECT
public:
    DataTypesListForm(QWidget *parent = 0);
    ~DataTypesListForm();
    void feedInDataTypes(const QVector<UserDataType*>& dataTypes);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DataTypesListForm *m_ui;
};

#endif // DATATYPESLISTFORM_H
