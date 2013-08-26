#ifndef GUI_DBMENU_H
#define GUI_DBMENU_H

#include <QString>
#include <QMap>
#include <QVector>

/**
 * @brief The DBMenu class
 * Built from the repository
 */
class DBMenu
{
public:

    enum MENU_TYPE
    {
        MENU_SINGLE = 0,
        MENU_SEPARATOR = 1,
        MENU_GROUP = 2
    };

public:

    DBMenu(const QString& text, MENU_TYPE type, const QString& icon, const QString& id);

    void addSibling(DBMenu* menuItem, int atPos);

    void addChild(DBMenu *menuItem);

    const QMap<int, DBMenu*>& getSiblings() const
    {
        return m_siblings;
    }

    QString getText() const
    {
        return m_text;
    }

    const QVector<DBMenu*>& getChildren() const
    {
        return m_children;
    }

    QString getAction() const
    {
        return m_id;
    }

    MENU_TYPE getType() const
    {
        return m_type;
    }

private:

    QString m_id;
    QString m_text;
    QString m_icon;
    MENU_TYPE m_type;
    QMap<int, DBMenu*> m_siblings;
    QVector<DBMenu*> m_children;
};

#endif // GUI_DBMENU_H
