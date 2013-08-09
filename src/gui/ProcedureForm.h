#ifndef PROCEDUREFORM_H
#define PROCEDUREFORM_H

#include "core_enums.h"
#include "core_StoredMethod.h"

#include <QWidget>
#include <QSplitter>

namespace Ui {
    class ProcedureForm;
}

class TextEditWithCodeCompletion;
class FrameForLineNumbers;
class Procedure;
class Connection;
class UserDataType;

class ProcedureForm : public QWidget
{
    Q_OBJECT

public:

    explicit ProcedureForm(Version *v, ProcedureFormMode m,
                           bool guided,
                           bool forced = false,
                           Connection* c = 0, QWidget *parent = 0);
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
    void onReturnTypeComboChange(QString);
    void onProcNameChange(QString);
    void onBriefDescrChange(QString);
    void onDescrChange();
    void onAddParameter();
    void onDeleteParameter();
    void onMoveUpParameter();
    void onMoveDownParameter();
    void onProcTargetLanguageChange(QString);
    void onSelectParameter(QTreeWidgetItem*,int);

private:
    QString getProcNameFromSql();
    void disableEditingControls(bool dis);
    void toggleGuidedCreationControls(bool guided);
    void updateSqlDueToParamChange();
    PROGRAMMING_LANGUAGES getTargetLanguage();

private:
    Ui::ProcedureForm *ui;
    TextEditWithCodeCompletion* m_textEdit;
    FrameForLineNumbers* m_frameForLineNumbers;
    StoredMethod* m_proc;
    bool m_forcedChange;
    ProcedureFormMode m_mode;
    Version* m_version;
    QVector<UserDataType*> m_availableDataTypes;
    bool m_init;
    bool m_guided;
    QSplitter* m_splitter;
    ParameterAndDescription* m_currentParameter;
};

#endif // PROCEDUREFORM_H
