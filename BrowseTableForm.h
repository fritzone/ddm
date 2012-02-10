#ifndef BROWSETABLEFORM_H
#define BROWSETABLEFORM_H

#include <QWidget>
#include <QSplitter>

#include "TextEditWithCodeCompletion.h"

namespace Ui {
    class BrowseTableForm;
}

class QTableView;
class Connection;

class BrowseTableForm : public QWidget
{
    Q_OBJECT

public:
    explicit BrowseTableForm(QWidget *parent, Connection* c, const QString& tab);
    ~BrowseTableForm();
    QTableView* getTable();
    void focusOnTextEdit();

protected:
    void changeEvent(QEvent *e);
    void resizeEvent(QResizeEvent *);

public slots:
    void onRunQuery();
    void onSaveQuery();
    void onLoadQuery();
    void textChanged();

private:

    QString retrieveCurrentQuery();

private:
    Ui::BrowseTableForm *ui;
    TextEditWithCodeCompletion *m_textEdit;
    FrameForLineNumbers* m_frameForLineNumbers;
    static int m_firstP;
    static int m_lastP;
    Connection* m_connection;
    QString m_tab;
    QSplitter* spl;
    static QString browseString ;
};

#endif // BROWSETABLEFORM_H
