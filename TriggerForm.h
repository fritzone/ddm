#ifndef TRIGGERFORM_H
#define TRIGGERFORM_H

#include <QWidget>

namespace Ui {
    class TriggerForm;
}

class TriggerForm : public QWidget
{
    Q_OBJECT

public:
    explicit TriggerForm(QWidget *parent = 0);
    ~TriggerForm();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::TriggerForm *ui;
};

#endif // TRIGGERFORM_H
