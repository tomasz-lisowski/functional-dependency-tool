#include <stdio.h>
#include <stdlib.h>

#include "norm.h"
#include "utils.h"

static bool check_fd_bcnf(func_dep_st *fd, candidate_keys_st *keys_primary)
{
    // If RHS is subset of LHS.
    if (subset_sorted(fd->rhs, fd->lhs, fd->rhs_len, fd->lhs_len, sizeof(symb_id_kt)))
    {
        return true;
    }

    // Check if at least one key is subset of LHS.
    bool keys_subset_lhs = false;
    for (uint32_t key_idx = 0; key_idx < keys_primary->key_count; key_idx += 1)
    {
        candidate_key_st *key = &keys_primary->keys[key_idx];
        keys_subset_lhs |= subset_sorted(key->symbs, fd->lhs, key->symbs_count, fd->lhs_len, sizeof(symb_id_kt));
    }
    return keys_subset_lhs;
}

static bool check_fd_3nf(func_dep_st *fd, candidate_keys_st *keys_primary)
{
    if (check_fd_bcnf(fd, keys_primary))
    {
        return true;
    }

    bool key_attrib_found = false;
    for (uint32_t key_idx = 0; key_idx < keys_primary->key_count; key_idx += 1)
    {
        if (key_attrib_found)
        {
            break;
        }
        candidate_key_st *key = &keys_primary->keys[key_idx];
        for (uint32_t key_symb_idx = 0; key_symb_idx < key->symbs_count; key_symb_idx += 1)
        {
            // Check if each key attribute is a subset of RHS.
            if (subset_sorted(&key->symbs[key_symb_idx], fd->rhs, 1, fd->rhs_len, sizeof(symb_id_kt)))
            {
                key_attrib_found = true;
                break;
            }
        }
    }

    // RHS does (not) contain an attribute from any key.
    return key_attrib_found;
}

bool check_bcnf(func_dep_info_st *func_deps_info, candidate_keys_st *keys_primary)
{
    for (uint32_t fd_idx = 0; fd_idx < func_deps_info->func_deps_count; fd_idx += 1)
    {
        func_dep_st *fd = &func_deps_info->func_deps[fd_idx];
        if (check_fd_bcnf(fd, keys_primary))
        {
            continue;
        }
        else
        {
            return false;
        }
    }
    return true;
}

bool check_3nf(func_dep_info_st *func_deps_info, candidate_keys_st *keys_primary)
{
    // Check if RHS contains a key attribute.
    for (uint32_t fd_idx = 0; fd_idx < func_deps_info->func_deps_count; fd_idx += 1)
    {
        func_dep_st *fd = &func_deps_info->func_deps[fd_idx];
        if (check_fd_3nf(fd, keys_primary))
        {
            continue;
        }
        else
        {
            return false;
        }
    }
    return true;
}
