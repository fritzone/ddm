#ifndef ENTERNOTETEXTDIALOG_H
#define ENTERNOTETEXTDIALOG_H

#include <QDialog>

namespace Ui {
    class EnterNoteTextDialog;
}

class EnterNoteTextDialog : public QDialog {
    Q_OBJECT
public:
    EnterNoteTextDialog(QWidget *parent = 0);
    ~EnterNoteTextDialog();

    QString getText();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::EnterNoteTextDialog *ui;
};

#endif // ENTERNOTETEXTDIALOG_H
