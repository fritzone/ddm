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

    CELLTYPE_NOTHING = 11

};


#endif // OPTIONSTYPE_H
