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
    void setText(const QString&);
    bool isFramed() const;
    void setFramed(bool);

public slots:

    void onChangeColor();
private:

    void setColorIcon();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::EnterNoteTextDialog *ui;

    QColor m_currentColor;
};

#endif // ENTERNOTETEXTDIALOG_H
