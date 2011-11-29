#include "QTextEditWithCodeCompletion.h"

#include "Workspace.h"
#include "Version.h"
#include "Table.h"
#include "Column.h"
#include "IconFactory.h"
#include "gui_colors.h"
#include "db_DatabaseEngine.h"

#include <QKeyEvent>
#include <QMessageBox>
#include <QDebug>

QTextEditWithCodeCompletion::QTextEditWithCodeCompletion(QWidget* p) : QTextEdit(p), m_lst(0), m_timer()
{
    m_lst = new QListWidgetForCodeCompletion(this);
    m_lst->setSelectionMode(QAbstractItemView::SingleSelection);
    m_lst->hide();
    m_timer.setInterval(300);
    QObject::connect(&m_timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    QObject::connect(m_lst, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onListItemDoubleClicked(QModelIndex)));
}

void QTextEditWithCodeCompletion::onTimer()
{
    if(!m_lst->isVisible())
    {
        QPoint p = cursorRect().bottomRight();
        qDebug() << p;
        m_lst->move(p);
        m_lst->show();
        m_timer.stop();
        populateCodeCompletionListbox();
    }
}

void QTextEditWithCodeCompletion::onListItemDoubleClicked(QModelIndex idx)
{
    QListWidgetItem* item = m_lst->itemFromIndex(idx);
    QString g = item->text();
    m_lst->hide();
    insertPlainText(g);
}

void QTextEditWithCodeCompletion::keyPressEvent(QKeyEvent *e)
{
    Qt::KeyboardModifiers m = e->modifiers();
    m_timer.stop();

    if(e->key() == Qt::Key_Period)
    {
        m_timer.start();
    }

    if(e->key() == Qt::Key_Comma)
    {
        m_timer.start();
    }

    if(m.testFlag(Qt::ControlModifier))
    {
        if(e->key() == Qt::Key_Space)
        {
            QPoint p = cursorRect().bottomRight();
            qDebug() << p;
            m_lst->move(p);
            m_lst->show();
            populateCodeCompletionListbox();
            return;
        }
    }

    if(e->key() == Qt::Key_Escape)
    {
        if(m_lst->isVisible())
        {
            m_lst->hide();
        }
    }

    if(e->key() == Qt::Key_Down)
    {
        if(m_lst->isVisible())
        {
            m_lst->selectNext();
            //return;
        }
    }

    QTextEdit::keyPressEvent(e);
}

void QTextEditWithCodeCompletion::populateCodeCompletionListboxWithTablesOfVersion()
{
    // WARNING!!! For easier code writing this is not (yet) following the OOP principles. For true OOP we would need to load only the table instances!
    const QVector<Table*> tabs = Workspace::getInstance()->workingVersion()->getTables();
    for(int i=0; i<tabs.size(); i++)
    {
        QListWidgetItem *lwi = new QListWidgetItem(m_lst);
        lwi->setText(tabs.at(i)->getName());
        QFont f = lwi->font();
        f.setBold(true);
        lwi->setFont(f);
        lwi->setForeground(QBrush(Qt::black)); // TODO: move all these colors in a header file, customizable :)
        lwi->setIcon(QIcon(IconFactory::getTablesIcon().pixmap(16,16)));
    }
}

void QTextEditWithCodeCompletion::populateCodeCompletionListboxWithColumnsOfTable(const QString& tabName)
{
    Table* t = Workspace::getInstance()->workingVersion()->getTable(tabName);
    QStringList tcs = t->fullColumns();
    for(int i=0; i<tcs.size(); i++)
    {
        Column* c = t->getColumn(tcs.at(i));
        if(!c) c = t->getColumnFromParents(tcs.at(i));
        if(!c) return;
        QListWidgetItem *lwi = new QListWidgetItem(m_lst);
        lwi->setText(c->getName());
        QFont f = lwi->font();
        f.setBold(true);
        lwi->setFont(f);
        lwi->setForeground(QBrush(columnColor)); // TODO: move all these colors in a header file, customizable :)
        lwi->setIcon(QIcon(IconFactory::getColumnIcon().pixmap(16,16)));
    }
}

void QTextEditWithCodeCompletion::populateCodeCompletionListbox()
{
    m_lst->clear();

    int cp = textCursor().position() - 1;
    int saveCp = cp;

    // 1. find the word which was before the cursor
    QString g = toPlainText();
    QString wordBeforeCursor = "";
    QString keywordBeforeCursor = "";
    bool thereWereSpaces = false;
    // remove all the spaces that are between the cursor and the last word to left
    while(cp && g.at(cp) == ' ')
    {
        thereWereSpaces = true;
        cp--;
    }

    // and find the word
    while(cp > -1 && g.at(cp) != ' ')
    {
        wordBeforeCursor.prepend(g.at(cp--));
    }

    qDebug() << wordBeforeCursor;

    // 2. Find the last keyword which was before the cursor
    bool foundKeyword = false;
    QStringList dbKeywords = Workspace::getInstance()->currentProjectsEngine()->getKeywords();
    while(!foundKeyword)
    {
        QString temp = "";
        while(cp && g.at(cp) == ' ') { cp--; }
        if(!cp) break;
        while(cp > -1 && g.at(cp) != ' ')
        {
            temp.prepend(g.at(cp--));
        }
        if(dbKeywords.contains(temp.toUpper(), Qt::CaseInsensitive))
        {
            foundKeyword = true;
            keywordBeforeCursor = temp;
        }
    }

    if(wordBeforeCursor.endsWith('.'))  // we arrived here after pressing a . and waiting a 300 milliseconds (or just Ctrl+space). This is supposed to be after a table
    {
        wordBeforeCursor.remove(wordBeforeCursor.length() - 1,1);
        qDebug() << wordBeforeCursor;

        // WARNING!!! For easier code writing this is not (yet) following the OOP principles. For true OOP we would need to load only the table instances!
        // But I'm jus curious to see it work
        if(Workspace::getInstance()->workingVersion()->hasTable(wordBeforeCursor))
        {
            populateCodeCompletionListboxWithColumnsOfTable(wordBeforeCursor);
        }
    }

    // we arrived here after ctrl+space
    // Now see what is left to us
    if(thereWereSpaces) // in this case the user might want to see the appropriate context for the current position. See the last keyword and decide
    {
        if(wordBeforeCursor.toUpper() == "FROM")    // from? Show a list of tables
        {
            populateCodeCompletionListboxWithTablesOfVersion();
        }
    }

    if(foundKeyword && !keywordBeforeCursor.isEmpty())  // the user pressed Ctrl+Space in the query, no necessarily after a keyword
    {
        if(keywordBeforeCursor.toUpper() == "FROM")
        {
            populateCodeCompletionListboxWithTablesOfVersion();
        }
    }
    m_lst->setFocus();

}
