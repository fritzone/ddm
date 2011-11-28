#include "QTextEditWithCodeCompletion.h"

#include "Workspace.h"
#include "Version.h"
#include "Table.h"
#include "Column.h"
#include "IconFactory.h"

#include <QKeyEvent>
#include <QMessageBox>
#include <QDebug>

QTextEditWithCodeCompletion::QTextEditWithCodeCompletion(QWidget* p)   : QTextEdit(p), m_lst(0), m_timer()
{
    m_lst = new QListWidget(this);
    m_lst->setSelectionMode(QAbstractItemView::SingleSelection);
    m_lst->hide();
    m_timer.setInterval(300);
    QObject::connect(&m_timer, SIGNAL(timeout()), this, SLOT(onTimer()));
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

void QTextEditWithCodeCompletion::keyPressEvent(QKeyEvent *e)
{
    Qt::KeyboardModifiers m = e->modifiers();
    m_timer.stop();

    if(e->key() == Qt::Key_Period)
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

    QTextEdit::keyPressEvent(e);
}

void QTextEditWithCodeCompletion::populateCodeCompletionListbox()
{
    int cp = textCursor().position() - 1;
    QString g = toPlainText();
    QString wordBeforeCursor = "";
    while(cp > -1 && g.at(cp) != ' ')
    {
        wordBeforeCursor.prepend(g.at(cp--));
    }

    qDebug() << wordBeforeCursor;

    if(wordBeforeCursor.endsWith('.'))
    {
        wordBeforeCursor.remove(wordBeforeCursor.length() - 1,1);
        qDebug() << wordBeforeCursor;

        // WARNING!!! For easier code writing this is not (yet) following the OOP principles. For true OOP we would need to load only the table instances!
        // But I'm jus curious to see it work
        if(Workspace::getInstance()->workingVersion()->hasTable(wordBeforeCursor))
        {
            Table* t = Workspace::getInstance()->workingVersion()->getTable(wordBeforeCursor);
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
                lwi->setForeground(QBrush(QColor(87,17,6))); // TODO: move all these colors in a header file, customizable :)
                lwi->setIcon(QIcon(IconFactory::getColumnIcon().pixmap(16,16)));
            }
        }
    }
}
