#ifndef QTEXTEDITWITHCODECOMPLETION_H
#define QTEXTEDITWITHCODECOMPLETION_H

#include <QTextEdit>
#include <QListWidget>
#include <QTimer>
#include <QObject>

#include "QListWidgetForCodeCompletion.h"

class QTextEditWithCodeCompletion : public QTextEdit
{
    Q_OBJECT

public:

    QTextEditWithCodeCompletion(QWidget *p);
    void keyPressEvent ( QKeyEvent * e );
    void insertText(const QString&);

private slots:
    void onTimer();
    void onListItemDoubleClicked(QModelIndex);

private:
    void populateCodeCompletionListbox();
    void populateCodeCompletionListboxWithTablesOfVersion();
    void populateCodeCompletionListboxWithColumnsOfTable(const QString&);

private:
    QListWidgetForCodeCompletion* m_lst;
    QTimer m_timer;
};

#endif // QTEXTEDITWITHCODECOMPLETION_H
