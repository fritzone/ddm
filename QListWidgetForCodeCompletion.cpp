#include "QListWidgetForCodeCompletion.h"
#include "QTextEditWithCodeCompletion.h"



void QListWidgetForCodeCompletion::keyPressEvent(QKeyEvent *event)
{
    if(event->key() != Qt::Key_Up && event->key() != Qt::Key_Down && event->key() != Qt::Key_Enter)
    {
        qobject_cast<QTextEditWithCodeCompletion*>(parent())->keyPressEvent(event);
        return;
    }

    if(event->key() == Qt::Key_Enter)
    {
        hide();
    }
    QListWidget::keyPressEvent(event);

}

void QListWidgetForCodeCompletion::selectNext()
{

}
