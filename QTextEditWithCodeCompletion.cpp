#include "QTextEditWithCodeCompletion.h"
#include "Workspace.h"
#include "Version.h"
#include "Table.h"
#include "Column.h"
#include "IconFactory.h"
#include "gui_colors.h"
#include "db_DatabaseEngine.h"
#include "db_DatabaseBuiltinFunction.h"
#include "SqlHighlighter.h"
#include "db_AbstractDTSupplier.h"
#include "core_Connection.h"
#include "BrowseTableForm.h"

#include <QKeyEvent>
#include <QMessageBox>
#include <QDebug>

int QTextEditWithCodeCompletion::TablePositionInText::colorCounter = -1;

static bool skippableSinceWhitespace(QChar c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

QTextEditWithCodeCompletion::QTextEditWithCodeCompletion(QWidget* p, Connection* c) : QTextEdit(p),
    m_lst(0), m_timer(), m_currentBgColor(Qt::white), m_highlighter(0), dbKeywords(), funcs(), m_tabs(), m_connection(c), m_browseForm(0)
{
    m_lst = new QListWidgetForCodeCompletion(this);
    m_lst->setSelectionMode(QAbstractItemView::SingleSelection);
    m_lst->hide();
    m_timer.setInterval(300);
    QObject::connect(&m_timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    QObject::connect(m_lst, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onListItemDoubleClicked(QModelIndex)));

    QFont font;
    font.setFamily(QString::fromUtf8("Courier"));
    font.setPointSize(11);
    setFont(font);

    if(Workspace::getInstance()->hasCurrentSolution())
    {
        m_highlighter = new SqlHighlighter(document(),
                                       Workspace::getInstance()->currentProjectsEngine()->getKeywords(),
                                       Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->numericTypes(),
                                       Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->booleanTypes(),
                                       Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->textTypes(),
                                       Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->blobTypes(),
                                       Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->dateTimeTypes(),
                                       Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->miscTypes(),
                                       Workspace::getInstance()->workingVersion()->getTables());
        dbKeywords = Workspace::getInstance()->currentProjectsEngine()->getKeywords();
        funcs = Workspace::getInstance()->currentProjectsEngine()->getBuiltinFunctions();
        QVector<Table*> t = Workspace::getInstance()->workingVersion()->getTables();
        for(int i=0; i<t.size(); i++)
        {
            m_tabs.append(t.at(i)->getName());
        }
    }

    if(c)
    {
        if(m_highlighter == 0)
        {
            m_highlighter = new SqlHighlighter(document(),
                                       c->getEngine()->getKeywords(),
                                       c->getEngine()->getDTSupplier()->numericTypes(),
                                       c->getEngine()->getDTSupplier()->booleanTypes(),
                                       c->getEngine()->getDTSupplier()->textTypes(),
                                       c->getEngine()->getDTSupplier()->blobTypes(),
                                       c->getEngine()->getDTSupplier()->dateTimeTypes(),
                                       c->getEngine()->getDTSupplier()->miscTypes(),
                                       QVector<Table*>());
            dbKeywords = c->getEngine()->getKeywords();
            funcs = c->getEngine()->getBuiltinFunctions();
            m_tabs = c->getTables();
        }
        else
        {
            m_tabs.append(c->getTables());
        }
    }
}

void QTextEditWithCodeCompletion::onTimer()
{
    if(!m_lst->isVisible())
    {
        QPoint p = cursorRect().bottomRight();
        //qDebug() << p;
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
    resetBackgrounds();
    insertText(g);
    QVariant v = item->data(Qt::UserRole);
    if(v.isValid())
    {
        QString n = v.toString();
        if(n.at(0) == '@')
        {
            insertFunctionParantheses();
        }
    }

}

void QTextEditWithCodeCompletion::insertFunctionParantheses()
{
    insertText("()");
    QTextCursor t = textCursor();
    t.setPosition(t.position() - 1);
    setTextCursor(t);

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
        int t = e->key();

        qDebug() << "key=" << e->key() << "enter = " << Qt::Key_Enter << " return = " << Qt::Key_Return;

        if(e->key() == Qt::Key_Space)
        {
            QPoint p = cursorRect().bottomRight();
            m_lst->move(p);
            m_lst->show();
            populateCodeCompletionListbox();
            return;
        }

        if(t == Qt::Key_Enter ||t == Qt::Key_Return) // Ctrl + Enter is supposed to run the query if we have a connection
        {
            if(m_connection)
            {
                if(m_browseForm)
                {
                    m_browseForm->onRunQuery();
                }
            }
        }
    }

    if(e->key() == Qt::Key_Escape)
    {
        if(m_lst->isVisible())
        {
            m_lst->hide();
            resetBackgrounds();
        }
    }

    if(m_lst->isVisible())
    {
        QTextEdit::keyPressEvent(e);
        populateCodeCompletionListbox();
        return;
    }

    QTextEdit::keyPressEvent(e);
}

void QTextEditWithCodeCompletion::populateCodeCompletionListboxWithTablesOfVersion(const QString& tabPrefix)
{
    // WARNING!!! For easier code writing this is not (yet) following the OOP principles. For true OOP we would need to load only the table instances!

    for(int i=0; i<m_tabs.size(); i++)
    {
        QString tabName = m_tabs.at(i);
        if(tabName.startsWith(tabPrefix) || tabName.indexOf(tabPrefix)!= -1)
        {
            QListWidgetItem *lwi = new QListWidgetItem(m_lst);
            lwi->setText(tabName);
            QFont f = lwi->font();
            f.setBold(true);
            lwi->setFont(f);
            lwi->setForeground(QBrush(Qt::black)); // TODO: move all these colors in a header file, customizable :)
            lwi->setIcon(QIcon(IconFactory::getTablesIcon().pixmap(16,16)));
        }
    }
}

void QTextEditWithCodeCompletion::populateCodeCompletionListboxWithColumnsOfTable(const QString& tabName, const QString& prefix)
{
    QStringList tcs;

    if(!Workspace::getInstance()->hasCurrentSolution())
    {
        tcs  = m_connection->getEngine()->getColumnsOfTable(m_connection, tabName);
    }
    else
    {
        Table* t = Workspace::getInstance()->workingVersion()->getTable(tabName);
        if(!t) return;
        tcs = t->fullColumns();
    }

    for(int i=0; i<tcs.size(); i++)
    {
        if(!tcs.at(i).startsWith(prefix)) continue;
        QString t = tcs.at(i);
        QList<QListWidgetItem*>  old = m_lst->findItems(t, Qt::MatchFixedString);
        QVariant v;
        v.setValue(tabName);
        if(old.size() > 0)
        {
            t = tabName + "." + t;
            for(int j=0; j<old.size(); j++)
            {
                QVariant v1 = old.at(j)->data(Qt::UserRole);
                if(v1.isValid())
                {
                    QString n = old.at(j)->text();
                    n = v1.toString() + "." + n;
                    old.at(j)->setText(n);
                }
            }
        }
        QListWidgetItem *lwi = new QListWidgetItem(m_lst);
        lwi->setText(t);
        lwi->setData(Qt::UserRole, v);
        QFont f = lwi->font();
        f.setBold(true);
        lwi->setFont(f);
        lwi->setBackgroundColor(m_currentBgColor);
        lwi->setForeground(QBrush(columnColor)); // TODO: move all these colors in a header file, customizable :)
        lwi->setIcon(QIcon(IconFactory::getColumnIcon().pixmap(16,16)));
    }

}

void QTextEditWithCodeCompletion::resetBackgrounds()
{
    for(int i=0; i<m_lastTablePositions.size(); i++)
    {
        QTextCursor origCursor = textCursor();
        QTextCursor copyCursor = origCursor;

        copyCursor.setPosition(m_lastTablePositions.at(i).b);
        for(int j = m_lastTablePositions.at(i).b; j!= m_lastTablePositions.at(i).e+1; j++) copyCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
        QTextCharFormat fmt = copyCursor.charFormat();
        fmt.setBackground(QBrush(Qt::white));
        copyCursor.setCharFormat(fmt);
        setTextCursor(origCursor);
    }
    m_lastTablePositions.clear();
    setFocus();

}

void QTextEditWithCodeCompletion::insertText(const QString & text)
{
    QString g = toPlainText();
    int cp = textCursor().position() - 1;
    QString wordBeforeCursor = "";
    // and find the word
    while(cp > -1 && g.at(cp) != ' ' && g.at(cp) != '.' && g.at(cp) != ',')
    {
        wordBeforeCursor.prepend(g.at(cp--));
    }
    // now match wordBeforeCursor to text and see from which position we need to include the "text"
    int i = 0;
    while(i <text.length() && i<wordBeforeCursor.length() && wordBeforeCursor.at(i) == text.at(i)) i++;
    insertPlainText(text.mid(i));
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
    // remove all the spaces that are between the cursor and the last word to left
    while(cp > 0 && g.at(cp) == ' ')
    {
        cp--;
    }

    // and find the word
    while(cp > -1 && g.at(cp) != ' ' && g.at(cp) != '=' && g.at(cp) != ')' && g.at(cp) != '(' && g.at(cp) != ',')
    {
        wordBeforeCursor.prepend(g.at(cp--));
    }

    //qDebug() << wordBeforeCursor;

    // 2. Find the last keyword which was before the cursor
    bool foundKeyword = false;

    if(dbKeywords.contains(wordBeforeCursor.toUpper(), Qt::CaseInsensitive))
    {
        foundKeyword = true;
        keywordBeforeCursor = wordBeforeCursor;
    }

    while(!foundKeyword && cp > -1)
    {
        QString temp = "";
        while(cp > -1 && skippableSinceWhitespace(g.at(cp))) { cp--; }
        if(cp == 0 || cp == -1) break;
        while(cp > -1 && ! skippableSinceWhitespace(g.at(cp)))
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
        // WARNING!!! For easier code writing this is not (yet) following the OOP principles. For true OOP we would need to load only the table instances!
        // But I'm jus curious to see it work
        if(Workspace::getInstance()->hasCurrentSolution())
        {
            if(Workspace::getInstance()->workingVersion()->hasTable(wordBeforeCursor))
            {
                QString empty("");
                populateCodeCompletionListboxWithColumnsOfTable(wordBeforeCursor, empty);
                m_lst->setFocus();
                return;
            }
        }
        else
        {
            // TODO: check the table from the vector we have
        }
    }

    if(wordBeforeCursor.indexOf(".") != -1)   // see if this is part of something longer, such as: PERSON.ADDR_[ID]
    {
        if(foundKeyword && !keywordBeforeCursor.isEmpty() && keywordBeforeCursor.toUpper() == "FROM")
        {
            return; // don't allow this in the FROM section
        }
        QString beforeDot = wordBeforeCursor.mid(0, wordBeforeCursor.indexOf("."));
        QString afterDot = wordBeforeCursor.mid(wordBeforeCursor.indexOf(".") + 1);
        // qDebug() << beforeDot << " " << afterDot;
        if(Workspace::getInstance()->hasCurrentSolution())
        {
            if(Workspace::getInstance()->workingVersion()->hasTable(beforeDot))  // see if there is a table for this
            {
                Table* t = Workspace::getInstance()->workingVersion()->getTable(beforeDot);
                QStringList tcs = t->fullColumns();
                for(int i=0; i<tcs.size(); i++)
                {
                    Column* c = t->getColumn(tcs.at(i));
                    if(!c) c = t->getColumnFromParents(tcs.at(i));
                    if(!c) return;
                    if(!c->getName().startsWith(afterDot)) continue;    // This is an extra line
                    QListWidgetItem *lwi = new QListWidgetItem(m_lst);
                    lwi->setText(c->getName());
                    QFont f = lwi->font();
                    f.setBold(true);
                    lwi->setFont(f);
                    lwi->setForeground(QBrush(columnColor)); // TODO: move all these colors in a header file, customizable :)
                    lwi->setIcon(QIcon(IconFactory::getColumnIcon().pixmap(16,16)));
                }
            }
        }
        else
        {
            // TODO: check the table from the vector
        }
        m_lst->setFocus();
        return;
    }


    if(foundKeyword && !keywordBeforeCursor.isEmpty())  // the user pressed Ctrl+Space in the query, no necessarily after a keyword
    {
        if(keywordBeforeCursor.toUpper() == "FROM")
        {
            QString prefix = wordBeforeCursor;
            if(wordBeforeCursor.trimmed().toUpper() == keywordBeforeCursor.trimmed().toUpper()) prefix = "";
            populateCodeCompletionListboxWithTablesOfVersion(prefix);
            m_lst->setFocus();
            return;
        }

        if(keywordBeforeCursor.toUpper() == "WHERE")
        {
            QStringList tabs = getTablesFromQuery();
            for(int i=0; i<tabs.size(); i++)
            {
                QListWidgetItem *lwi = new QListWidgetItem(m_lst);
                lwi->setText(tabs.at(i));
                QFont f = lwi->font();
                f.setBold(true);
                lwi->setFont(f);
                lwi->setForeground(QBrush(Qt::black)); // TODO: move all these colors in a header file, customizable :)
                lwi->setIcon(QIcon(IconFactory::getTablesIcon().pixmap(16,16)));
            }
            m_lst->setFocus();
            return;
        }

        if(keywordBeforeCursor.toUpper() == "SELECT")
        {
            // ok, Fetch all the tables from the "FROM" part of the query and create a list box based on the columns. Also keep an open eye for functions
            QStringList tables = getTablesFromQuery();
            m_currentBgColor = Qt::white;
            for(int i=0; i<tables.size(); i++)
            {

                // TODO: Mask the columns from different tables with different backgrounds, also try the same thing in the text edit, ie. change the background of the tables
                QString prefix = wordBeforeCursor;
                if(wordBeforeCursor.trimmed().toUpper() == keywordBeforeCursor.trimmed().toUpper()) prefix = "";
                populateCodeCompletionListboxWithColumnsOfTable(tables.at(i), prefix);
                QTextCursor origCursor = textCursor();
                QTextCursor copyCursor = origCursor;

                copyCursor.setPosition(m_lastTablePositions.at(i).b);
                for(int j = m_lastTablePositions.at(i).b; j!= m_lastTablePositions.at(i).e+1; j++) copyCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
                QTextCharFormat fmt = copyCursor.charFormat();
                fmt.setBackground(QBrush(m_currentBgColor));
                copyCursor.setCharFormat(fmt);
                setTextCursor(origCursor);
                m_currentBgColor = m_lastTablePositions.at(i).c;

            }

            // now add the functions but only if we have started typing something

            for(int i=0; i<funcs.size(); i++)
            {
                QString prefix = wordBeforeCursor;
                if(wordBeforeCursor.trimmed().toUpper() == keywordBeforeCursor.trimmed().toUpper()) prefix = "";
                if(! (funcs.at(i).getName().mid(1).toUpper().startsWith(prefix))) continue;
                QListWidgetItem *lwi = new QListWidgetItem(m_lst);
                lwi->setText(funcs.at(i).getName().mid(1));
                QFont f = lwi->font();
                lwi->setData(Qt::UserRole, QVariant(funcs.at(i).getName()));

                f.setItalic(true);
                lwi->setFont(f);
                lwi->setBackgroundColor(Qt::white);
                lwi->setForeground(Qt::black); // TODO: move all these colors in a header file, customizable :)
                lwi->setIcon(QIcon(IconFactory::getFunctionIcon().pixmap(16,16)));
            }
            m_lst->setFocus();
            return;
        }

        if(keywordBeforeCursor.toUpper() == "BY"); // anything before a by will be ignored. It'd too complicated for the user
    }
}

QStringList QTextEditWithCodeCompletion::getTablesFromQuery()
{
    QString g = toPlainText();
    int l = g.length();
    int i = g.toUpper().indexOf("FROM");
    QStringList tables;
    m_lastTablePositions.clear();
    if(i == -1)
    {
        // add the functions
    }
    else
    {
        i += 4; // skip the from
        bool stillCanGo = true;
        QTextEditWithCodeCompletion::TablePositionInText::colorCounter = 0;
        while(i < g.length() && stillCanGo)
        {
            if(i < g.length() - 4)
            {
                QString next = QString(g.at(i).toUpper()) + g.at(i+1).toUpper() + g.at(i+2).toUpper() + g.at(i+3).toUpper() + g.at(i+4).toUpper();
                stillCanGo = (next != "WHERE");
            }
            if(stillCanGo)
            {
                // now find the table names
                QString currentTable = "";
                while(i < g.length() && skippableSinceWhitespace(g.at(i))) i ++; // skip the spaces after the "from" or the next table
                TablePositionInText next;
                next.b = i;
                while(i < g.length() && (g.at(i).isLetter() || g.at(i).isDigit() || g.at(i) == '_') )  currentTable += g.at(i ++);   // the table name
                next.e = i-1;
                next.c = next.nextColor();

                m_lastTablePositions.append(next);
                while(i < g.length() && (g.at(i) == ' ' || g.at(i) == ',' || g.at(i) == ';')) i ++; // skip everything till the next table, or end
                tables << currentTable;
            }
        }
    }
    return tables;
}


QColor QTextEditWithCodeCompletion::TablePositionInText::nextColor()
{
    // TODO: This is a little bit naive approach... Make a way to create a "shade" of various colors and put those in the list It'll look better
    static QVector<QColor> colors;
    colors.clear();
    colors << color1 << color2 << color3<< color4<< color5<< color6<< color7<< color8<< color9<< color10<< color11<< color12<< color13<< color14<< color15;

    return colors[++ colorCounter % colors.size()];
}

void QTextEditWithCodeCompletion::resetToConnection(Connection* c)
{
    m_connection = c;
    m_tabs= m_connection->getTables();
}
