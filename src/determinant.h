#pragma once

#include <stdint.h>

#include "attrib_closure.h"
#include "func_dep.h"
#include "key.h"

typedef struct determinant_info_s
{
    symb_id_kt *attrib_set;
    uint32_t attrib_set_count;
    attrib_closure_arr_st *determinants;
} determinant_info_st;

/**
 * @brief Given a set of attributes Z (passed inside @p det_info ), finds all minimal sets of attributes that
 * functionally determine at least the provided set Z. Minimal meaning that no determinant set is a subset of another
 * determinant set.
 * @param closures_all A list of all possible closures for the given functional dependencies. This can be computed using
 * `keys_compute'.
 * @param det_info Where the determinants will be written. It is assumed that it also includes the set of attributes
 * that this function will comnpute determinants of.
 * @return 0 on success, >0 on failure.
 */
uint32_t determinants_compute(attrib_closure_arr_st *closures_all, determinant_info_st *det_info);
