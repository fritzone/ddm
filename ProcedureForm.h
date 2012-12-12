#ifndef PROCEDUREFORM_H
#define PROCEDUREFORM_H

#include "enums.h"
#include "core_StoredMethod.h"

#include <QWidget>

namespace Ui {
    class ProcedureForm;
}

class TextEditWithCodeCompletion;
class FrameForLineNumbers;
class Procedure;
class Connection;

class ProcedureForm : public QWidget
{
    Q_OBJECT

public:

    explicit ProcedureForm(Version *v, ProcedureFormMode m, bool forced = false, Connection* c = 0, QWidget *parent = 0);
    ~ProcedureForm();
    void setProcedure(StoredMethod* p);
    void initSql();
    void showSql();

protected:
    void changeEvent(QEvent *e);

private slots:
    void textChanged();
    void onLockUnlock(bool);
    void onUndelete();
    void onSave();
    void onLoadFile();
    void onInject();
    void onNew();

private:
    QString getProcNameFromSql();
    void disableEditingControls(bool dis);

private:
    Ui::ProcedureForm *ui;
    TextEditWithCodeCompletion* m_textEdit;
    FrameForLineNumbers* m_frameForLineNumbers;
    StoredMethod* m_proc;
    bool m_forcedChange;
    ProcedureFormMode m_mode;
    Version* m_version;
};

#endif // PROCEDUREFORM_H
