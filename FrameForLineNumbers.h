#ifndef FRAMEFORLINENUMBERS_H
#define FRAMEFORLINENUMBERS_H

#include <QFrame>

class FrameForLineNumbers : public QFrame
{
    Q_OBJECT
public:
    explicit FrameForLineNumbers(QWidget *parent = 0);

    void setNumber(int i, int firstv);


protected:

    void paintEvent(QPaintEvent *);

signals:

public slots:
private:
    int m_nr;
    int m_firstv;
};

#endif // FRAMEFORLINENUMBERS_H
