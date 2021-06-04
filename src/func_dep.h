#pragma once

#include <stdint.h>

typedef char symb_kt;
typedef uint32_t symb_id_kt;

typedef struct func_dep_s
{
    symb_id_kt *lhs;
    symb_id_kt *rhs;
    uint32_t lhs_len; // Number of elements in LHS array.
    uint32_t rhs_len; // Number of elements in RHS array.
} func_dep_st;

// XXX: The actual symbol (character) is never used in computations.
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

/**
 * @brief Print a `func_dep_st' struct.
 * @param format The desired format of the output (0 = {A} -> {B}, 1 = A-->B, 2 = A -> B, default is verbose).
 * @param func_dep The functioan ldependency that will be printed.
 * @param attrib_dict The attribute dictionary that will be used to translate symbol IDs to symbols.
 */
void print_func_dep(uint8_t format, func_dep_st *func_dep, attrib_dict_st *attrib_dict);

/**
 * @brief Print out all functional dependencies in a list in a specified format.
 * @param format Same as `format' parameter of `print_func_dep'.
 * @param func_deps_info What functional dependencies to print.
 */
void print_func_deps(uint8_t format, func_dep_info_st *func_deps_info);

/**
 * @brief Lookup an ID of a symbol.
 * @param symb What symbol to lookup.
 * @param attr_dict Dictionary to use for lookup.
 * @return ID of @p symb.
 */
symb_id_kt symb_to_id(symb_kt symb, attrib_dict_st *attr_dict);

/**
 * @brief Looks up what symbol has a given ID.
 * @param symb_id ID to lookup.
 * @param attr_dict Dictionary to use for lookup.
 * @return Symbol identified by @p id .
 */
symb_kt id_to_symb(symb_id_kt symb_id, attrib_dict_st *attr_dict);

/**
 * @brief Compares two `symb_id_kt' elements and returns comparison outcome using flags from `comp_flag_e'.
 * @param a Pointer to a `symb_id_kt' element. First operand (a < b, a == b, a > b).
 * @param b Pointer to a `symb_id_kt' element. Second operand (a < b, a == b, a > b).
 * @return ORd flags defined by `comp_flag_e' or 0.
 */
uint8_t comp_symb_id(void *const a, void *const b);
