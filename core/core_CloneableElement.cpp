#include "core_CloneableElement.h"
#include "uids.h"

CloneableElement::CloneableElement() : m_sourceUid(nullUid)
{
}

void CloneableElement::setSourceUid(const QUuid &o)
{
    m_sourceUid = o;
}

QUuid CloneableElement::getSourceUid() const
{
    return m_sourceUid;
}
