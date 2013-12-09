#ifndef INSERTNEWRECORDDIALOG_H
#define INSERTNEWRECORDDIALOG_H

#include <QDialog>

namespace Ui {
class InsertNewRecordDialog;
}

class Connection;

class InsertNewRecordDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit InsertNewRecordDialog(QWidget *parent,
                                   const QString& tab,
                                   Connection* c);

    ~InsertNewRecordDialog();
    
private:
    Ui::InsertNewRecordDialog *ui;
};

#endif // INSERTNEWRECORDDIALOG_H
