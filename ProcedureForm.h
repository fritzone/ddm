#ifndef PROCEDUREFORM_H
#define PROCEDUREFORM_H

#include <QWidget>

namespace Ui {
    class ProcedureForm;
}

class ProcedureForm : public QWidget
{
    Q_OBJECT

public:
    explicit ProcedureForm(QWidget *parent = 0);
    ~ProcedureForm();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ProcedureForm *ui;
};

#endif // PROCEDUREFORM_H
