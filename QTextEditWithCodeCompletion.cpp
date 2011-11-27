#include "QTextEditWithCodeCompletion.h"
#include <QKeyEvent>
#include <QMessageBox>
void QTextEditWithCodeCompletion::keyPressEvent(QKeyEvent *e)
{
    Qt::KeyboardModifiers m = e->modifiers();

    if(m | Qt::ControlModifier)
    {
        if(e->key() == Qt::Key_Space)
        {
            QMessageBox::critical(this, "aaa", "bbb", QMessageBox::Ok);
        }
    }

    QTextEdit::keyPressEvent(e);
}
