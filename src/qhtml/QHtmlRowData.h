#ifndef QHTMLROWDATA_H
#define QHTMLROWDATA_H

#include "QHtmlBodyElements.h"

class QHtmlRowData : public QHtmlBodyElements
{
public:
    QHtmlRowData(QHtmlBodyElements* data);
    virtual QString html() const;

private:
    QHtmlBodyElements* m_data;
};

#endif // QHTMLROWDATA_H
