#ifndef ABOUTBOXDIALOG_H
#define ABOUTBOXDIALOG_H

#include <QDialog>

namespace Ui {
    class AboutBoxDialog;
}

class AboutBoxDialog : public QDialog {
    Q_OBJECT
public:
    AboutBoxDialog(QWidget *parent = 0);
    ~AboutBoxDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::AboutBoxDialog *ui;
};

#endif // ABOUTBOXDIALOG_H
