#ifndef OPTIONSTYPE_H
#define OPTIONSTYPE_H

enum OptionsType
{
    OPTIONS_DUPLICATE = 0,
    OPTIONS_NEW_TABLE = 1,
    OPTIONS_NEW_SUBQUERY = 2,
    OPTIONS_ADD_FROM = 3,
    OPTIONS_ADD_WHERE = 4,
    OPTIONS_ADD_GROUPBY = 5,
    OPTIONS_ADD_HAVING = 6,
    OPTIONS_ADD_ORDERBY = 7,
    OPTIONS_ADD_JOIN = 8,
    OPTIONS_AS = 9,
    OPTIONS_NEW_WHERE_EXPR = 10,

    OPTIONS_LAST
};


enum CellTypeChooserType
{
    // * small type choosers

    // ** used in a from query
    CELLTYPE_TABLE = 0,

    // ** used in a where condition
    CELLTYPE_NOT = 1,           // !
    CELLTYPE_MINUS = 2,         // -
    CELLTYPE_NEGATE = 3,        // ~
    CELLTYPE_PLUS = 4,          // +
    CELLTYPE_BINARY = 5,        // BINARY (!!) This is MySQL specific?
    CELLTYPE_EXISTS = 6,

    // * big type choosers
    // ** used in a wher condition
    CELLTYPE_COLUMN = 7,
    CELLTYPE_FUNCTION = 8,
    CELLTYPE_LITERAL = 9,
    CELLTYPE_QUERY = 10,

    CELLTYPE_NOTHING = 11,
    CELLTYPE_REMOVE_THIS = 12,      // plain close button :)
    CELLTYPE_FUNCTION_EXPAND = 13,  // when the QueryGraphicsHelper sees this it asks for the DB engine to provide a list of functions supported

    CELLTYPE_CURSOR = 14,           // this is always placed after the last cell type chooser in the unary expression


    CELLTYPE_LAST

};

enum FunctionType
{
    FT_CONTROLFLOW = 0,
    FT_STRING = 1,
    FT_NUMERIC = 2,
    FT_DATETIME = 3,
    FT_FULLTEXTSEARCH = 4,
    FT_CAST = 5,
    FT_BIT = 6,
    FT_CRYPT = 7,
    FT_INFO = 8,
    FT_MISC = 9,
    FT_INVALID = 10
};

#endif // OPTIONSTYPE_H
