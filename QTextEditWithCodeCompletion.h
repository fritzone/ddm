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


private:

    struct TablePositionInText
    {
        int b;
        int e;
        QColor c;
        static QColor nextColor();
        static int colorCounter;
    };

public:

    QTextEditWithCodeCompletion(QWidget *p);
    void keyPressEvent ( QKeyEvent * e );
    void insertText(const QString&);
    void resetBackgrounds();
    void insertFunctionParantheses();

private slots:
    void onTimer();
    void onListItemDoubleClicked(QModelIndex);

private:
    void populateCodeCompletionListbox();
    void populateCodeCompletionListboxWithTablesOfVersion(const QString& tabPrefix);
    void populateCodeCompletionListboxWithColumnsOfTable(const QString& tabName, const QString& prefix);
    QStringList getTablesFromQuery();

private:
    QListWidgetForCodeCompletion* m_lst;
    QTimer m_timer;
    QColor m_currentBgColor;
    QVector<TablePositionInText> m_lastTablePositions;
};

#endif // QTEXTEDITWITHCODECOMPLETION_H
