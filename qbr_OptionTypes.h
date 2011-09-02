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
    OPTIONS_NEW_WHERE_EXPR_OR = 10,
    OPTIONS_NEW_WHERE_EXPR_AND = 11,
    OPTIONS_NEW_WHERE_EXPR = 12,

    OPTIONS_LAST
};


enum CellTypeChooserType
{
    // * small type choosers

    // ** used in a from query
    CELLTYPE_TABLE = 0,

    // ** used in a where condition

    /* Logical operations  -- all these below one button */
    CELLTYPE_NOT,           // !
    CELLTYPE_NEGATE,        // ~
    CELLTYPE_XOR,           // ^
    CELLTYPE_LSHIFT,        // <<
    CELLTYPE_RSHIFT,        // >>
    CELLTYPE_OR,            // |
    CELLTYPE_AND,           // &

    /* Mathematical operations  -- all these below one button */
    CELLTYPE_PLUS,          // +
    CELLTYPE_MINUS,         // -
    CELLTYPE_MULTIPLY,      // *
    CELLTYPE_DIVIDE,        // /
    CELLTYPE_DIV,           // DIV
    CELLTYPE_MOD,           // %

    /* Comparison operators -- they have their own button too*/
    CELLTYPE_EQUAL,
    CELLTYPE_NOTEQUAL,
    CELLTYPE_LESS,
    CELLTYPE_GREATER,
    CELLTYPE_LESS_OR_EQUAL,
    CELLTYPE_GREATER_OR_EQUAL,

    /* BETWEEN keyword will get own button*/
    CELLTYPE_BETWEEN,       // BETWEEN keyword

    /* The LIKE keyword will get its own button */
    CELLTYPE_LIKE,          // The LIKE keyword

    /* Special keywords - all these go to their own button*/
    CELLTYPE_BINARY,        // BINARY (!!) This is MySQL specific?
    CELLTYPE_EXISTS,        // The EXISTS keyword
    CELLTYPE_IS,            // The IS keyword
    CELLTYPE_IN,            // The IN keyword
    CELLTYPE_NOT_TEXT,      // The NOT keyword

    /* The functions from the database, they will have one button. Maybe the menu will be broken up like: most frequently used, mathematical, string, etc...*/
    CELLTYPE_FUNCTION,      // a function call

    /* The columns will get a button too - If in a where query these can be got only from the FROM query*/
    CELLTYPE_COLUMN,        // A Column from a table

    CELLTYPE_LITERAL,       // A typed in value
    CELLTYPE_QUERY,         // A query ... huhh

    CELLTYPE_CURSOR,           // this is always placed after the last cell type chooser in the unary expression or where the functions place their parameters
    CELLTYPE_OPEN_PARANTHESES_FOR_FUNCTION_CALL,
    CELLTYPE_CLOSE_PARANTHESES_FOR_FUNCTION_CALL,
    CELLTYPE_COMMA,

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
