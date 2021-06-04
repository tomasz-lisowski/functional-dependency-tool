#pragma once

#include <stdbool.h>

#include "func_dep.h"
#include "key.h"

/**
 * @brief Checks if a set of functional deps is in Boyce–Codd normal form (BCNF).
 * @param func_deps_info The set of functional dependencies to check.
 * @param keys_primary A list of pre-computed primary keys for @p func_deps_info . This can be computed using
 * `keys_compute'.
 * @return TRUE if func deps in @p fund_deps_info are in BCNF, FALSE if not.
 */
bool check_bcnf(func_dep_info_st *func_deps_info, key_arr_st *keys_primary);

/**
 * @brief Checks if a set of functional deps is in 3rd normal form (3NF).
 * @param func_deps_info The set of functional dependencies to check.
 * @param keys_primary A list of pre-computed primary keys for @p func_deps_info . This can be computed using
 * `keys_compute'.
 * @return TRUE if func deps in @p fund_deps_info are in 3NF, FALSE if not.
 */
bool check_3nf(func_dep_info_st *func_deps_info, key_arr_st *keys_primary);
