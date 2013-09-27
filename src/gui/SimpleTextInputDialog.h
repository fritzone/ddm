#ifndef SIMPLETEXTINPUTDIALOG_H
#define SIMPLETEXTINPUTDIALOG_H

#include <QDialog>

namespace Ui {
    class SimpleTextInputDialog;
}

class SimpleTextInputDialog : public QDialog {
    Q_OBJECT
public:
    SimpleTextInputDialog(QWidget *parent, const QString& caption, const QString &text);
    ~SimpleTextInputDialog();
    void setText(const QString& txt);
    QString getText();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::SimpleTextInputDialog *ui;
};

#endif // SIMPLETEXTINPUTDIALOG_H
