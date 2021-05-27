#pragma once

#include <stdbool.h>

#include "common.h"

enum comp_flag_e
{
    COMP_EQUAL = 1 << 0,   // 1=equal and 0=unequal
    COMP_GREATER = 1 << 1, // 1=greater and 0=smaller
};

void print_func_dep(uint8_t format, func_dep_st *func_dep, attrib_dict_st *attrib_dict);

/**
 * @brief Print out all functional dependencies in a list in a specified format.
 * @param format The desired format of the output (0 = {A} -> {B}, 1 = A-->B, 2 = A -> B, default is verbose).
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
 * @brief Checks if @p set_a is a subset of or equal to @p set_b where both sets are sorted.
 * @param set_a Set which must be sorted in ascending order.
 * @param set_b Set which must be sorted in ascending order.
 * @param set_a_len Number of elements in @p set_a .
 * @param set_b_len Number of elements in @p set_b .
 * @param el_size Size (in bytes) of an element in either set.
 * @return `true' if @p set_a is a subset or or equal to @p set_b .
 */
bool subset_sorted(void *set_a, void *set_b, uint32_t set_a_len, uint32_t set_b_len, uint32_t el_size);

uint8_t comp_symb_id(void *const a, void *const b);

void sort(void *data, uint32_t el_count, uint32_t el_size, uint8_t (*comparator)(void *const, void *const));

void flip(void *data, uint32_t el_count, uint32_t el_size);
