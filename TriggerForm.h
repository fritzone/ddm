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

class TriggerForm : public QWidget
{
    Q_OBJECT

public:
    explicit TriggerForm(bool reverseSource, bool fc, QWidget *parent = 0);
    ~TriggerForm();
    void setTrigger(Trigger* t);
    void initSql();
    void feedInTables(const QVector<Table*>& tables);
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

protected:
    void changeEvent(QEvent *e);

private:
    Ui::TriggerForm *ui;
    Trigger* m_trigger;
    TextEditWithCodeCompletion* m_textEdit;
    FrameForLineNumbers* m_frameForLineNumbers;
    bool m_forcedChange;
    bool m_reverseSource;
};

#endif // TRIGGERFORM_H
