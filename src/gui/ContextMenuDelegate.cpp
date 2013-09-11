#include "ContextMenuEnabledTreeWidget.h"
#include "ContextMenuCollection.h"
#include "ClipboardFactory.h"

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
            ContextMenuEnabledTreeWidgetItem* item = m_tree->itemAt(mouseEvent->pos());

            if(item)
            {
                m_tree->setLastRightclickedItem(item);

                // now a big ugly piece of code, disabling or enabling actions, regardless of the selected menu, depending on what's on the clipboard
                ContextMenuCollection::getInstance()->getAction_PasteTable()->setEnabled(ClipboardFactory::tableIsAvailableOnClipboard());
                ContextMenuCollection::getInstance()->getAction_PasteColumn()->setEnabled(ClipboardFactory::columnIsAvailableOnClipboard());

                // and finally show the table
                m_contextMenu->showContextMenu(mouseEvent->globalPos(), index, item);
            }
            return true;
        }
    }

    return false;
}
