#ifndef DIAGRAMOBJECTDESCRIPTOR_H
#define DIAGRAMOBJECTDESCRIPTOR_H

#include <QString>

class DiagramObjectDescriptor
{
public:

    DiagramObjectDescriptor(const QString& txt, int x, int y) : m_txt(txt), m_x(x), m_y(y) {}


    QString getText() const
    {
        return m_txt;
    }

    int getX() {return m_x;}
    int getY() {return m_y;}

    void setX(int x)
    {
        m_x = x;
    }

    void setY(int y)
    {
        m_y = y;
    }

private:

    // this is: for objects isntantiated from this class the table name.
    // for obejcts derived from the Note descriptor the tect of the note.
    // for objects derived from the FK descriptor the name of the first table
    QString m_txt;
    int m_x;
    int m_y;
};

#endif // DIAGRAMOBJECTDESCRIPTOR_H
