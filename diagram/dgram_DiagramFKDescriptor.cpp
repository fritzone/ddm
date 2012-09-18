#include "dgram_DiagramFKDescriptor.h"

CloneableElement* DiagramFKDescriptor::clone(Version* sourceVersion, Version* targetVersion)
{
    DiagramFKDescriptor* dfk = new DiagramFKDescriptor(getText(), getText2(),
                                                       getX(), getY(),
                                                       m_ellipsePos.x(),  m_ellipsePos.y(),
                                                       line1OtherPoint.x(),  line1OtherPoint.y(),
                                                       line1PosPoint.x(), line1PosPoint.y(),
                                                       line2OtherPoint.x(),  line2OtherPoint.y(),
                                                       line2PosPoint.x(), line2PosPoint.y(),
                                                       rel1TxtPos.x(), rel1TxtPos.y(),
                                                       rel2TxtPos.x(), rel2TxtPos.y(),
                                                       arrowP1.x(), arrowP1.y(),
                                                       arrowP2.x(),  arrowP2.y(),
                                                       arrowPos.x(),  arrowPos.y(),
                                                       getFkName(), targetVersion);

    dfk->setSourceUid(getObjectUid());
    return dfk;
}
