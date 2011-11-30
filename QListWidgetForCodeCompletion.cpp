#include "QListWidgetForCodeCompletion.h"
#include "QTextEditWithCodeCompletion.h"



void QListWidgetForCodeCompletion::keyPressEvent(QKeyEvent *event)
{
    int t = event->key();
    if(t != Qt::Key_Up && t != Qt::Key_Down && t != Qt::Key_Enter && t != Qt::Key_Return)
    {
        qobject_cast<QTextEditWithCodeCompletion*>(parent())->keyPressEvent(event);
        return;
    }

    if(t == Qt::Key_Return || t == Qt::Key_Enter)
    {
        QString g = item(0)->text();
        if(selectedItems().length())
        {
            g = selectedItems().at(0)->text();
        }
        hide();
        parentWidget()->setFocus(Qt::OtherFocusReason);
        qobject_cast<QTextEditWithCodeCompletion*>(parent())->insertText(g);
    }
    else
    {
        QListWidget::keyPressEvent(event);
    }

}
