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
    explicit TriggerForm(QWidget *parent = 0);
    ~TriggerForm();
    void setTrigger(Trigger* t);
    void initSql();
    void feedInTables(const QVector<Table*>& tables);

protected slots:
    void textChanged();
protected:
    void changeEvent(QEvent *e);

private:
    Ui::TriggerForm *ui;
    Trigger* m_trigger;
    TextEditWithCodeCompletion* m_textEdit;
    FrameForLineNumbers* m_frameForLineNumbers;
    bool m_forcedChange;

};

#endif // TRIGGERFORM_H
