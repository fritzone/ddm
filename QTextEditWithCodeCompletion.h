#ifndef QTEXTEDITWITHCODECOMPLETION_H
#define QTEXTEDITWITHCODECOMPLETION_H

#include <QTextEdit>
#include <QListWidget>

class QTextEditWithCodeCompletion : public QTextEdit
{
public:

    QTextEditWithCodeCompletion(QWidget *p) : QTextEdit(p), m_lst(0)
    {}

    void keyPressEvent ( QKeyEvent * e );

private:

    QListWidget* m_lst;
};

#endif // QTEXTEDITWITHCODECOMPLETION_H
