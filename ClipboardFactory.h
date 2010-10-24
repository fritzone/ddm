#ifndef CLIPBOARDFACTORY_H
#define CLIPBOARDFACTORY_H

class Table;

/**
 * This class creates objects from the clipboard
 */
class ClipboardFactory
{
public:
    ClipboardFactory();

public:

    static Table* pasteTable();
    static bool tableIsAvailableOnClipboard();
};

#endif // CLIPBOARDFACTORY_H
