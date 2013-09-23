#include "gui_DBMenu.h"

DBMenu::DBMenu(const QString &text, MENU_TYPE type, const QString &icon, const QString &id)
    : m_id(id), m_text(text), m_icon(icon), m_type(type), m_siblings(), m_children()
{
}

void DBMenu::addChild(DBMenu *menuItem)
{
    m_children.append(menuItem);
}

void DBMenu::addSibling(DBMenu *menuItem, int atPos)
{
    m_siblings[atPos] = menuItem;
}
