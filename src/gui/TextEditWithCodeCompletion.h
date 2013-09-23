#ifndef QTEXTEDITWITHCODECOMPLETION_H
#define QTEXTEDITWITHCODECOMPLETION_H

#include <QPlainTextEdit>
#include <QListWidget>
#include <QTimer>
#include <QObject>
#include <QVector>
#include <QFrame>

#include "db_DatabaseBuiltinFunction.h"
#include "ListWidgetForCodeCompletion.h"

class SqlHighlighter;
class Connection;
class Table;
class BrowseTableForm;
class FrameForLineNumbers;

class TextEditWithCodeCompletion : public QPlainTextEdit
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

    TextEditWithCodeCompletion(QWidget *p = 0, Connection* c = 0);
    void keyPressEvent ( QKeyEvent * e );
    void insertText(const QString&);
    void resetBackgrounds();
    void insertFunctionParantheses();
    void setBrowseForm(BrowseTableForm* f)
    {
        m_browseForm = f;
    }
    void resetToConnection(Connection* c);
    void setLineNumberFrame(FrameForLineNumbers* f)
    {
        m_frameForLineNumbers = f;
    }
    void updateLineNumbers();

    void disableRow(int row)
    {
        m_disabledRows.append(row);
    }

    void enableRow(int row)
    {
        m_disabledRows.remove(m_disabledRows.indexOf(row));
    }

private slots:
    void onTimer();
    void onListItemDoubleClicked(QModelIndex);
    void onVScroll(int);

private:
    void populateCodeCompletionListbox();
    void populateCodeCompletionListboxWithTablesOfVersion(const QString& tabPrefix);
    void populateCodeCompletionListboxWithColumnsOfTable(const QString& tabName, const QString& prefix);
    QStringList getTablesFromQuery();

private:
    ListWidgetForCodeCompletion* m_lst;
    QTimer m_timer;
    QColor m_currentBgColor;
    QVector<TablePositionInText> m_lastTablePositions;
    SqlHighlighter* m_highlighter;
    QStringList dbKeywords;
    QVector<DatabaseBuiltinFunction> funcs;
    QStringList m_tabs;
    Connection* m_connection;
    BrowseTableForm* m_browseForm;
    FrameForLineNumbers *m_frameForLineNumbers;
    QVector<int> m_disabledRows;
};

#endif // QTEXTEDITWITHCODECOMPLETION_H
