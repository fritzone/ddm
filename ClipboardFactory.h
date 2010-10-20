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
};

#endif // CLIPBOARDFACTORY_H
