#ifndef OPTIONSTYPE_H
#define OPTIONSTYPE_H

enum OptionsType
{
    OPTIONS_DUPLICATE = 0,
    OPTIONS_NEW_TABLE,
    OPTIONS_NEW_SUBQUERY,
    OPTIONS_ADD_FROM,
    OPTIONS_ADD_WHERE,
    OPTIONS_ADD_GROUPBY,
    OPTIONS_ADD_HAVING,
    OPTIONS_ADD_ORDERBY,
    OPTIONS_ADD_JOIN,
    OPTIONS_AS,
    OPTIONS_NEW_WHERE_EXPR_OR,
    OPTIONS_NEW_WHERE_EXPR_AND,
    OPTIONS_NEW_WHERE_EXPR,
    OPTIONS_NEW_COLUMN,
    OPTIONS_NEW_COLUMN_ORDER,
    OPTIONS_NEW_ALIAS,          // use in the order by

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

    /* parantheses for building*/
    CELLTYPE_OPEN_PARANTHESES,
    CELLTYPE_CLOSE_PARANTHESES,

    /* BETWEEN keyword will get own button*/
    CELLTYPE_BETWEEN,       // BETWEEN keyword

    /* The LIKE keyword will get its own button */
    CELLTYPE_LIKE,          // The LIKE keyword

    /*The Distinct keyword will get its own button too*/
    CELLTYPE_DISTINCT,

    /* The '*' is widely used in the queries, he will get its own button */
    CELLTYPE_STAR,

    /* Rollup keyword */
    CELLTYPE_ROLLUP,

    /* The NULL keyword */
    CELLTYPE_NULL,

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

    CELLTYPE_QUERY_OR,      // this goes in between queries: tab.col like 'asd' OR tab.col like 'bsd'
    CELLTYPE_QUERY_AND,

    CELLTYPE_LAST

};

enum FunctionType
{
    FT_CONTROLFLOW = 0,                     // CASE will not be supported in 0.1f ... too complex.
    FT_STRING,                              // DONE
    FT_NUMERIC,                             // DONE
    FT_DATETIME,                            // DONE
    FT_FULLTEXTSEARCH,                      // WILL NOT BE SUPPORTED IN RELEASE 0.1f
    FT_CAST,
    FT_BIT,
    FT_CRYPT,
    FT_INFO,
    FT_MISC,
    FT_AGGREGATE,                           // DONE
    FT_INVALID
};

#endif // OPTIONSTYPE_H
