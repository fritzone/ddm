#ifndef QHTMLTABLEHEADER_H
#define QHTMLTABLEHEADER_H

#include "QHtmlBodyElements.h"

class QHtmlTableHeader : public QHtmlBodyElements
{
public:
    QHtmlTableHeader(QHtmlBodyElements* data);
    virtual QString html() const;
private:
    QHtmlBodyElements* m_data;
};

#endif // QHTMLTABLEHEADER_H
