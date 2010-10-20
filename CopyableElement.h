#ifndef COPYABLEELEMENT_H
#define COPYABLEELEMENT_H

/**
 * An element which can be copied to the clipboard.
 */
class CopyableElement
{
public:

    virtual void copy() = 0;
};

#endif // COPYABLEELEMENT_H
