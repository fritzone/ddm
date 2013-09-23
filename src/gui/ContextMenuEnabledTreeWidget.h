#ifndef CONTEXTMENUENABLEDTREEWIDGET_H
#define CONTEXTMENUENABLEDTREEWIDGET_H

#include <QTreeWidget>
#include <QContextMenuEvent>
#include <QItemDelegate>
#include <QMouseEvent>
#include <QPoint>
#include <QMenu>
#include <qobject.h>
#include <qdebug.h>

class ContextMenuEnabledTreeWidgetItem : public QTreeWidgetItem
{
public:
    ContextMenuEnabledTreeWidgetItem(ContextMenuEnabledTreeWidgetItem* parent, QStringList items) : QTreeWidgetItem(parent, items), m_popupMenu(0)
    {}

    void setPopupMenu(QMenu* menu)
    {
        m_popupMenu = menu;
    }

    QMenu* contextMenu() const
    {
        return m_popupMenu;
    }

    virtual void setData(int column, int role, const QVariant &value)
    {
        QTreeWidgetItem::setData(column, role, value);
    }

private:

    QMenu* m_popupMenu;
};

class ContextMenuEnabledTreeWidget : public QTreeWidget
{

public:

    ContextMenuEnabledTreeWidget() : QTreeWidget() , m_rightclickedItem(0)
    {}

    ContextMenuEnabledTreeWidgetItem* itemAt(const QPoint& pos)
    {
        return dynamic_cast<ContextMenuEnabledTreeWidgetItem*>(QTreeWidget::itemAt(pos));
    }

    void setLastRightclickedItem(ContextMenuEnabledTreeWidgetItem* item)
    {
        m_rightclickedItem = item;
    }

    ContextMenuEnabledTreeWidgetItem* getLastRightclickedItem()
    {
        return m_rightclickedItem;
    }

private:

    // the item which received the last right click (ie: context menu)
    ContextMenuEnabledTreeWidgetItem* m_rightclickedItem;

};

class ContextMenuHandler
{
public:

    ContextMenuHandler() {}

    bool showContextMenu(const QPoint& pos, const QModelIndex&, ContextMenuEnabledTreeWidgetItem* item) const
    {
        if(item->contextMenu())
        {
            item->contextMenu()->exec(pos);
        }
        return true;
    }
};

class ContextMenuDelegate: public QItemDelegate
{
public:
    ContextMenuDelegate(ContextMenuHandler *const contextMenu, ContextMenuEnabledTreeWidget *const parent );

    bool editorEvent(QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index );

private:
    ContextMenuEnabledTreeWidget* m_tree;
    ContextMenuHandler *const m_contextMenu;
};

#endif // CONTEXTMENUENABLEDTREEWIDGET_H
