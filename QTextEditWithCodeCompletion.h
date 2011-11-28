#ifndef QTEXTEDITWITHCODECOMPLETION_H
#define QTEXTEDITWITHCODECOMPLETION_H

#include <QTextEdit>
#include <QListWidget>
#include <QTimer>
#include <QObject>

class QTextEditWithCodeCompletion : public QTextEdit
{
    Q_OBJECT

public:

    QTextEditWithCodeCompletion(QWidget *p);
    void keyPressEvent ( QKeyEvent * e );

private slots:
    void onTimer();

private:
    void populateCodeCompletionListbox();

private:
    QListWidget* m_lst;
    QTimer m_timer;
};

#endif // QTEXTEDITWITHCODECOMPLETION_H
