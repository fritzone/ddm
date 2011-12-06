#include "QListWidgetForCodeCompletion.h"
#include "QTextEditWithCodeCompletion.h"



void QListWidgetForCodeCompletion::keyPressEvent(QKeyEvent *event)
{
    int t = event->key();
    if(t != Qt::Key_Up && t != Qt::Key_Down && t != Qt::Key_Enter && t != Qt::Key_Return && t != Qt::Key_PageDown && t!= Qt::Key_PageUp && t!= Qt::Key_Home && t!=Qt::Key_End)
    {
        qobject_cast<QTextEditWithCodeCompletion*>(parent())->keyPressEvent(event);
        return;
    }

    if(t == Qt::Key_Return || t == Qt::Key_Enter)
    {
        QString g = item(0)->text();
        QListWidgetItem* itm = 0;
        QVariant v;
        if(selectedItems().length())
        {
            itm = selectedItems().at(0);
            g = itm->text();
            v = itm->data(Qt::UserRole);
        }
        hide();
        parentWidget()->setFocus(Qt::OtherFocusReason);
        qobject_cast<QTextEditWithCodeCompletion*>(parent())->insertText(g);
        qobject_cast<QTextEditWithCodeCompletion*>(parent())->resetBackgrounds();

        if(v.isValid())
        {
            QString n = v.toString();
            if(n.at(0) == '@')
            {
                qobject_cast<QTextEditWithCodeCompletion*>(parent())->insertFunctionParantheses();
            }
        }

    }
    else
    {
        QListWidget::keyPressEvent(event);
    }

}
