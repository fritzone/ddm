#ifndef PROCEDUREFORM_H
#define PROCEDUREFORM_H

#include <QWidget>

class QTextEditWithCodeCompletion;
class FrameForLineNumbers;
class Procedure;

namespace Ui {
    class ProcedureForm;
}

class ProcedureForm : public QWidget
{
    Q_OBJECT

public:
    explicit ProcedureForm(QWidget *parent = 0);
    ~ProcedureForm();
    void setProcedure(Procedure* p)
    {
        m_proc = p;
    }
    void initSql();
    void showSql();

protected:
    void changeEvent(QEvent *e);

private slots:
    void textChanged();

private:
    QString getProcNameFromSql();

private:
    Ui::ProcedureForm *ui;
    QTextEditWithCodeCompletion* m_textEdit;
    FrameForLineNumbers* m_frameForLineNumbers;
    Procedure* m_proc;
    bool m_forcedChange;
};

#endif // PROCEDUREFORM_H
