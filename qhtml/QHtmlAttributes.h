#ifndef _QHTMLATTRIBUTES_H_
#define                                                                                                                                                                                                                                                                          _QHTMLATTRIBUTES_H_

#include <QString>

namespace attributes
{

class QHtmlAttribute
{
public:
    QHtmlAttribute(const QString& name);
};

static const QHtmlAttribute dir("dir");

}
#endif
