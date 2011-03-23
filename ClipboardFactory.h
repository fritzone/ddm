#ifndef CLIPBOARDFACTORY_H
#define CLIPBOARDFACTORY_H

class Table;
class Column;

/**
 * This class creates objects from the clipboard
 */
class ClipboardFactory
{
public:
    ClipboardFactory();

public:
    // paste a table
    static Table* pasteTable();
    static bool tableIsAvailableOnClipboard();

    //paste a column
    static Column* pasteColumn();
    static bool columnIsAvailableOnClipboard();

};

#endif // CLIPBOARDFACTORY_H
