#ifndef TRIGGERFORM_H
#define TRIGGERFORM_H

#include <QWidget>
#include <QVector>

namespace Ui {
    class TriggerForm;
}

class Trigger;
class TextEditWithCodeCompletion;
class FrameForLineNumbers;
class Table;
class Version;
class Connection;

class TriggerForm : public QWidget
{
    Q_OBJECT

public:
    explicit TriggerForm(Version *v, Connection *c, bool reverseSource, bool fc, QWidget *parent);
    ~TriggerForm();
    void setTrigger(Trigger* t);
    void initSql();
    void feedInTables(const QVector<QString> &tables, bool oop);
    void feedInTables(const QStringList& tables);
    void feedInTriggerEvents(const QStringList&);
    void feedInTriggerTimes(const QStringList&);
    void showSql();

protected slots:
    void textChanged();
    void nameChanged(QString);
    void whenChanged(QString);
    void eventChanged(QString);
    void tableChanged(QString);
    void descriptionChanged();
    void onLockUnlock(bool);
    void onUndelete();

protected:
    void changeEvent(QEvent *e);

private:
    void disableEditingControls(bool);

private:
    Ui::TriggerForm *ui;
    Trigger* m_trigger;
    TextEditWithCodeCompletion* m_textEdit;
    FrameForLineNumbers* m_frameForLineNumbers;
    bool m_forcedChange;
    bool m_reverseSource;
    Version* m_version;
};

#endif // TRIGGERFORM_H
