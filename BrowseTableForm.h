#ifndef BROWSETABLEFORM_H
#define BROWSETABLEFORM_H

#include <QWidget>
#include "QTextEditWithCodeCompletion.h"

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

protected:
    void changeEvent(QEvent *e);

public slots:
    void onRunQuery();

private:

    QString retrieveCurrentQuery();

private:
    Ui::BrowseTableForm *ui;
    static QTextEditWithCodeCompletion *m_textEdit;
    static FrameForLineNumbers* m_frameForLineNumbers;
    static int m_firstP;
    static int m_lastP;
    Connection* m_connection;
    QString m_tab;
};

#endif // BROWSETABLEFORM_H
