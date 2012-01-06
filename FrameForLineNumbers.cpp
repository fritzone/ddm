#include "FrameForLineNumbers.h"
#include <QPainter>
#include <QDebug>
#include <QFont>

FrameForLineNumbers::FrameForLineNumbers(QWidget *parent) : QFrame(parent), m_nr(0), m_firstv(0)
{
    setUpdatesEnabled(true);
}

void FrameForLineNumbers::setNumber(int i, int firstv)
{
    m_nr = i;
    m_firstv = firstv;
    qDebug() << "bbb" << i << " fff: "<< firstv;

    update();

}

void FrameForLineNumbers::paintEvent(QPaintEvent *)
{
    QPainter a(this);
    a.setPen(Qt::black);
    QFont f;
    f.setFamily("Courier");
    f.setBold(true);
    f.setPixelSize(16);
    a.setFont(f);
    int y = 16;
    for(int i=m_firstv; i<m_nr; i++)
    {
        QString s = QString::number(i + 1);
        a.drawText(10, y, s);
        y += 19;
    }
    qDebug() << "aaa";
    a.end();
}
