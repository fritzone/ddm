#include "TreeItem.h"

#include <QFont>

void TreeItem::updateGui()
{
    QFont f = m_location->font(0);
    if(lockState() == LockableElement::LOCKED || lockState() == LockableElement::FINAL_LOCK) f.setItalic(true); else f.setItalic(false);
    if(lockState() == LockableElement::FINAL_LOCK) m_location->setForeground(0, QBrush(Qt::lightGray));
    m_location->setFont(0, f);
}
