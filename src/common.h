#pragma once

#include <stdint.h>

typedef char symb_kt;
typedef uint32_t symb_id_kt;

typedef struct func_dep_s
{
    symb_kt *lhs;
    symb_kt *rhs;
    uint32_t lhs_len; // Number of elements in LHS array.
    uint32_t rhs_len; // Number of elements in RHS array.
} func_dep_st;

typedef struct attrib_dict_s
{
    symb_kt *symbs;     // List of symbols. The symbol index is used as an ID.
    uint32_t symbs_len; // Number of symbols.
} attrib_dict_st;

typedef struct func_dep_info_s
{
    func_dep_st *func_deps;      // Array of functional dependencies.
    attrib_dict_st *attrib_dict; // Attribute dictionary used for interpreting this array.
    uint32_t func_deps_count;    // Number of elements in functional dependencies array.
} func_dep_info_st;
