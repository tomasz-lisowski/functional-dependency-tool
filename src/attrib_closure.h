#pragma once

#include <stdint.h>

#include "common.h"

typedef struct attribute_closure_s
{
    symb_id_kt *attrib_set;  // List of attributes for which the closure is stored in the struct.
    symb_id_kt *closure;     // List of all IDs of symbols in the closure of the attributes in the attribute set.
    uint32_t attrib_set_len; // Number of elements in the attribute set.
    uint32_t closure_len;    // Number of elements in the closure.
} attrib_closure_st;

/**
 * @brief Compute the closure of attributes given a list of functional dependencies.
 * @param closure Where the closure will be stored. When passed, this function expects that the attribute set is filled
 * in already and uses it as a definition for what closure to compute.
 * @param func_deps_info Functional dependency info.
 * @return 0 on success, >0 on failure.
 */
uint32_t attrib_closure_compute(attrib_closure_st *closure, func_dep_info_st *func_deps_info);