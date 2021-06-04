#pragma once

#include <stdbool.h>

#include "func_dep.h"

enum comp_flag_e
{
    COMP_EQUAL = 1 << 0,   // 1=equal and 0=unequal
    COMP_GREATER = 1 << 1, // 1=greater and 0=smaller
};

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

/**
 * @brief Perform insertion sort on an array of elements of any size or type given that a way to compare the elements
 * has been provided via @p comparator .
 * @param data The data that will be sorted.
 * @param el_count Number of elements in @p data .
 * @param el_size Size of each element in @p data .
 * @param comparator Function which is able to compare two elements in @p data and return `comp_flag_e' flags.
 */
void sort(void *data, uint32_t el_count, uint32_t el_size, uint8_t (*comparator)(void *const, void *const));

/**
 * @brief Flip an array of elements i.e. make it such that an iteration from the back to front is equivalent to a front
 * to back iteration before the flip.
 * @param data The list that will be flipped.
 * @param el_count Number of elements in @p data .
 * @param el_size Size of each element in @p data .
 */
void flip(void *data, uint32_t el_count, uint32_t el_size);
