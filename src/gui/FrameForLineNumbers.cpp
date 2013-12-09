#include "FrameForLineNumbers.h"
#include <QPainter>
#include <QDebug>
#include <QFont>

FrameForLineNumbers::FrameForLineNumbers(QWidget *parent) : QFrame(parent), m_nrs()
{
    setUpdatesEnabled(true);

    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(sizePolicy.hasHeightForWidth());
    setSizePolicy(sizePolicy);
    setMinimumSize(QSize(48, 0));
    setMaximumSize(QSize(48, 16777215));
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Raised);
}

void FrameForLineNumbers::beginLineNumbers()
{
    m_nrs.clear();
}

void FrameForLineNumbers::addLineNumber(int nr, int y, bool disabled)
{
    NumberPosition np(nr, y, disabled);
    m_nrs.append(np);
}

void FrameForLineNumbers::endLineNumbers()
{
    update();
}

void FrameForLineNumbers::paintEvent(QPaintEvent *)
{
    QPainter a(this);

    QFont f;
    f.setFamily("Courier");
    f.setBold(false);
    int TSIZE= 16;
    f.setPixelSize(TSIZE);
    a.setFont(f);

    for(int i=0; i<m_nrs.size(); i++)
    {
        QString s = QString::number(m_nrs.at(i).nr);
        if(m_nrs.at(i).nr < 10) s = "000" + s;
        else
        if(m_nrs.at(i).nr < 100) s = "00" + s;
        else
        if(m_nrs.at(i).nr < 1000) s = "0" + s;

        QRect r(0, m_nrs.at(i).y, this->rect().width(), TSIZE);
        if(m_nrs[i].disabled)
        {
            a.setPen(Qt::darkGray);
        }
        else
        {
            a.setPen(Qt::black);
        }
        a.drawText(r, s);

        QRect anotherRect(this->rect().width() - 8, m_nrs.at(i).y + 5, 7, 7);
        a.drawRect(anotherRect);

   }

    a.end();
}
