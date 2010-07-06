#ifndef DIAGRAMOBJECTDESCRIPTOR_H
#define DIAGRAMOBJECTDESCRIPTOR_H

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

private:

    QString m_txt;
    int m_x;
    int m_y;
};

#endif // DIAGRAMOBJECTDESCRIPTOR_H
