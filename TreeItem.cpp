#include "TreeItem.h"

#include <QFont>

void TreeItem::updateGui()
{
    QFont f = m_location->font(0);
    if(isLocked()) f.setItalic(true); else f.setItalic(false);
    m_location->setFont(0, f);
}
