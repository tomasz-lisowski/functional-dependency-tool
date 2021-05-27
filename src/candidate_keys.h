#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "attrib_closure.h"
#include "common.h"

typedef enum key_type_e
{
    KEY_NOT = 1,          // Not a key.
    KEY_PRIMARY = 1 << 1, // Can be a primary key.
    KEY_SUPER = 1 << 2,   // Is a superset of a primary key.
} key_type_et;

typedef struct candidate_key_s
{
    symb_id_kt *symbs;
    uint32_t symbs_count; // Number of symbol IDs in symbs list.
    key_type_et key_type;
} candidate_key_st;

typedef struct candidate_keys_s
{
    candidate_key_st *keys;
    uint32_t key_count;
} candidate_keys_st;

/**
 * @brief Compute a selected set of keys for a set of functional depenendencies.
 * @param keys Where a list of all keys whose types are contained in @p key_types , will be written.
 * @param closures_all This list will be filled with all possible closures given the attribute dictionary. The whole
 * list gets computed anyways in order to find the desired keys so it is unavoidable and it's more useful to return it.
 * @param fd_info Information regarding the functional dependencies.
 * @param key_types What key types to include in the @p keys list.
 * @return 0 on success, >0 on failure.
 */
uint32_t keys_compute(candidate_keys_st *keys, attrib_closure_arr_st *closures_all, func_dep_info_st *fd_info,
                      key_type_et key_types);

void print_key(candidate_key_st *key, attrib_dict_st *attrib_dict, bool type_show);
void print_key_arr(candidate_keys_st *keys, attrib_dict_st *attrib_dict, bool type_show);