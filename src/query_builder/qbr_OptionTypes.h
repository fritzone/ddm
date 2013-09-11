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
    CELLTYPE_TABLE = 0,                                                                                                             // 0

    // ** used in a where condition

    /* Logical operations  -- all these below one button */
    CELLTYPE_NOT,           // !                                                                                                    // 1
    CELLTYPE_NEGATE,        // ~                                                                                                    // 2
    CELLTYPE_XOR,           // ^                                                                                                    // 3
    CELLTYPE_LSHIFT,        // <<                                                                                                   // 4
    CELLTYPE_RSHIFT,        // >>                                                                                                   // 5
    CELLTYPE_OR,            // |                                                                                                    // 6
    CELLTYPE_AND,           // &                                                                                                    // 7

    /* Mathematical operations  -- all these below one button */
    CELLTYPE_PLUS,          // +                                                                                                    // 8
    CELLTYPE_MINUS,         // -                                                                                                    // 9
    CELLTYPE_MULTIPLY,      // *                                                                                                    // 10
    CELLTYPE_DIVIDE,        // /                                                                                                    // 11
    CELLTYPE_DIV,           // DIV                                                                                                  // 12
    CELLTYPE_MOD,           // %v                                                                                                   // 13

    /* Comparison operators -- they have their own button too*/
    CELLTYPE_EQUAL,         // ==                                                                                                    // 14
    CELLTYPE_NOTEQUAL,      // !=                                                                                                    // 15
    CELLTYPE_LESS,          // <                                                                                                     // 16
    CELLTYPE_GREATER,       // >                                                                                                     // 17
    CELLTYPE_LESS_OR_EQUAL, // <=                                                                                                    // 18
    CELLTYPE_GREATER_OR_EQUAL,// >=                                                                                                  // 19

    /* parantheses for building*/
    CELLTYPE_OPEN_PARANTHESES, // (                                                                                                  // 20
    CELLTYPE_CLOSE_PARANTHESES, // )                                                                                                 // 21

    /* BETWEEN keyword will get own button*/
    CELLTYPE_BETWEEN,       // BETWEEN keyword                                                                                       // 22

    /* The LIKE keyword will get its own button */
    CELLTYPE_LIKE,          // The LIKE keyword                                                                                      // 23

    /*The Distinct keyword will get its own button too*/
    CELLTYPE_DISTINCT,                                                                                                               // 24

    /* The '*' is widely used in the queries, he will get its own button */
    CELLTYPE_STAR,          // *                                                                                                     // 25

    /* Rollup keyword */
    CELLTYPE_ROLLUP,        // ROLLUP                                                                                                // 26

    /* The NULL keyword */
    CELLTYPE_NULL,          // NULL                                                                                                  // 27

    /* Special keywords - all these go to their own button*/
    CELLTYPE_BINARY,        // BINARY (!!) This is MySQL specific?                                                                   // 28
    CELLTYPE_EXISTS,        // The EXISTS keyword                                                                                    // 29
    CELLTYPE_IS,            // The IS keyword                                                                                        // 30
    CELLTYPE_IN,            // The IN keyword                                                                                        // 31
    CELLTYPE_NOT_TEXT,      // The NOT keyword                                                                                       // 32

    /* The functions from the database, they will have one button. Maybe the menu will be broken up like: most frequently used, mathematical, string, etc...*/
    CELLTYPE_FUNCTION,      // a function call                                                                                       // 33

    /* The columns will get a button too - If in a where query these can be got only from the FROM query*/
    CELLTYPE_COLUMN,        // A Column from a table                                                                                 // 34

    CELLTYPE_LITERAL,       // A typed in value                                                                                      // 35
    CELLTYPE_QUERY,         // A query ... huhh                                                                                      // 36

    CELLTYPE_CURSOR,           // this is always placed after the last cell type chooser in the unary expression or where the functions place their parameters
    CELLTYPE_OPEN_PARANTHESES_FOR_FUNCTION_CALL,
    CELLTYPE_CLOSE_PARANTHESES_FOR_FUNCTION_CALL,
    CELLTYPE_COMMA,

    CELLTYPE_QUERY_OR,      // this goes in between queries: tab.col like 'asd' OR tab.col like 'bsd'
    CELLTYPE_QUERY_AND,

    CELLTYPE_LAST

};

#endif // OPTIONSTYPE_H
