#include "ContextMenuEnabledTreeWidget.h"

#include <QMouseEvent>


ContextMenuDelegate::ContextMenuDelegate(ContextMenuHandler *const contextMenu, ContextMenuEnabledTreeWidget *const parent ) : QItemDelegate(parent) , m_tree(parent), m_contextMenu(contextMenu)
{
}

bool ContextMenuDelegate::editorEvent(QEvent * event, QAbstractItemModel * , const QStyleOptionViewItem & , const QModelIndex & index )
{
    if((event->type()==QEvent::MouseButtonPress) && index.isValid())
    {
        QMouseEvent *const mouseEvent = dynamic_cast<QMouseEvent*>(event);
        if(mouseEvent && (mouseEvent->button()==Qt::RightButton))
        {
            // TODO: decide whether we should select the right clicked stuff
            //m_tree->itemAt(mouseEvent->pos())->setSelected(true);

            ContextMenuEnabledTreeWidgetItem* item = m_tree->itemAt(mouseEvent->pos());

            if(item)
            {
                m_tree->setLastRightclickedItem(item);
                m_contextMenu->showContextMenu(mouseEvent->globalPos(), index, item);
            }

            return true;
        }
    }

    return false;
}
