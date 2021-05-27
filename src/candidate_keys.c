#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "candidate_keys.h"
#include "utils.h"

/**
 * @brief Tells us if @p keys contains a key which is a subset of @p key .
 * @param keys List of keys to be checked.
 * @param key Key where the subset should be found.
 * @return `true' if @p keys contains a key which is a subset of @p key, else `false'.
 */
static bool key_subset_contained(candidate_keys_st *keys, candidate_key_st *key)
{
    for (uint32_t keys_idx = 0; keys_idx < keys->key_count; keys_idx += 1)
    {
        candidate_key_st *key_tmp = &keys->keys[keys_idx];
        // Only check keys that are shorter than the provided key.
        if (key_tmp->symbs_count > 0 && key->symbs_count > key_tmp->symbs_count)
        {
            if (subset_sorted(key_tmp->symbs, key->symbs, key_tmp->symbs_count, key->symbs_count, sizeof(symb_id_kt)))
            {
                return true;
            }
        }
    }
    return false;
}

void print_key(candidate_key_st *key, attrib_dict_st *attrib_dict, bool type_show)
{
    printf("{");
    for (uint32_t symb_idx = 0; symb_idx < key->symbs_count; symb_idx += 1)
    {
        printf("%c", id_to_symb(key->symbs[symb_idx], attrib_dict));
        if (symb_idx + 1 < key->symbs_count)
        {
            printf(",");
        }
    }
    printf("}");

    if (type_show)
    {
        printf(" ");
        switch (key->key_type)
        {
        case KEY_NOT:
            printf("NOT KEY");
            break;
        case KEY_PRIMARY:
            printf("PRIMARY KEY");
            break;
        case KEY_SUPER:
            printf("SUPER KEY");
            break;
        default:
            printf("UNK");
            break;
        }
    }
    printf("\n");
}

void print_key_arr(candidate_keys_st *keys, attrib_dict_st *attrib_dict, bool type_show)
{
    for (uint32_t key_idx = 0; key_idx < keys->key_count; key_idx += 1)
    {
        print_key(&keys->keys[key_idx], attrib_dict, type_show);
    }
}

uint32_t keys_compute(candidate_keys_st *keys, attrib_closure_arr_st *closures_all, func_dep_info_st *fd_info,
                      key_type_et key_types)
{
    assert(keys != NULL);
    assert(keys->keys == NULL);
    assert(keys->key_count == 0);
    assert(fd_info != NULL);
    assert(fd_info->func_deps != NULL);
    assert(fd_info->attrib_dict != NULL);
    assert(closures_all != NULL);
    assert(closures_all->closures == NULL);
    assert(closures_all->closures_count == 0);

    // There are (2^n) combinations of attributes (of any length).
    const uint32_t closure_count_all = (1 << fd_info->attrib_dict->symbs_len);
    closures_all->closures = calloc(1, closure_count_all * sizeof(attrib_closure_st));

    // Start with single attribute sets.
    closures_all->closures[0].attrib_set = NULL;
    closures_all->closures[0].attrib_set_count = 0;
    closures_all->closures_count += 1;

    // Create all possible closures and compute them.
    {
        // This algo works by extending the length of all attribute sets with the previous length by
        // appending a symbol at the end. To avoid duplicates, only the set with the last length is considered.

        // Indicate the subset of closures with the last length.
        uint32_t closures_len_last_start = 0;
        uint32_t closures_len_last_end = closures_all->closures_count;
        for (uint32_t attrib_set_len = 0; attrib_set_len < fd_info->attrib_dict->symbs_len; attrib_set_len += 1)
        {
            for (uint32_t closure_idx = closures_len_last_start;
                 closure_idx < closures_len_last_start + (closures_len_last_end - closures_len_last_start);
                 closure_idx += 1)
            {
                attrib_closure_st *closure = &closures_all->closures[closure_idx];

                // XXX: Using fact that symbol ID is also the index of the symbol in the dictionary.
                uint32_t symb_idx_start =
                    closure->attrib_set_count == 0 ? 0 : closure->attrib_set[closure->attrib_set_count - 1] + 1;
                if (symb_idx_start >= fd_info->attrib_dict->symbs_len)
                {
                    // No symbol can be appended because the last symbol in this closure is the last symbol in the
                    // symbol dictionary.
                    continue;
                }

                for (uint32_t symb_idx = symb_idx_start; symb_idx < fd_info->attrib_dict->symbs_len; symb_idx += 1)
                {
                    attrib_closure_st *closure_new = &closures_all->closures[closures_all->closures_count++];

                    // Make space for one extra symbol in the new closure.
                    closure_new->attrib_set_count = closure->attrib_set_count + 1;
                    closure_new->attrib_set = calloc(closure_new->attrib_set_count, sizeof(symb_id_kt));

                    // Don't copy attributes from empty list.
                    if (closure->attrib_set_count > 0)
                    {
                        memcpy(closure_new->attrib_set, closure->attrib_set,
                               closure->attrib_set_count * sizeof(symb_id_kt));
                    }
                    closure_new->attrib_set[closure_new->attrib_set_count - 1] =
                        symb_to_id(fd_info->attrib_dict->symbs[symb_idx], fd_info->attrib_dict);
                    attrib_closure_compute(closure_new, fd_info);
                }
            }

            closures_len_last_start = closures_len_last_end;
            closures_len_last_end = closures_all->closures_count;
        }
    }

    // Determine what key type each generated set (using the above) is.
    {
        // Temporary key stores which can later be used to quickly copy memory chunks into final keys list.
        candidate_keys_st keys_primary = {0};
        keys_primary.key_count = 0;
        keys_primary.keys = calloc(sizeof(candidate_key_st), closures_all->closures_count);
        candidate_keys_st keys_super = {0};
        keys_super.key_count = 0;
        keys_super.keys = calloc(sizeof(candidate_key_st), closures_all->closures_count);
        candidate_key_st key_tmp = {0};
        key_tmp.symbs = calloc(sizeof(symb_id_kt), fd_info->attrib_dict->symbs_len);
        key_tmp.symbs_count = 0;
        key_tmp.key_type = KEY_NOT;

        for (uint32_t closure_idx = 0; closure_idx < closures_all->closures_count; closure_idx += 1)
        {
            attrib_closure_st *closure = &closures_all->closures[closure_idx];

            // Update temporary key.
            memcpy(key_tmp.symbs, closure->attrib_set, closure->attrib_set_count * sizeof(symb_id_kt));
            key_tmp.symbs_count = closure->attrib_set_count;

            // Final info about the key being analyzed.
            key_type_et key_final_type = KEY_NOT;
            candidate_key_st *key_final = NULL;

            if (closure->closure_len == fd_info->attrib_dict->symbs_len &&
                key_subset_contained(&keys_primary, &key_tmp))
            {
                key_final = &keys_super.keys[keys_super.key_count++];
                key_final_type = KEY_SUPER;
            }
            else if (closure->closure_len == fd_info->attrib_dict->symbs_len)
            {
                key_final = &keys_primary.keys[keys_primary.key_count++];
                key_final_type = KEY_PRIMARY;
            }

            if (key_final_type != KEY_NOT && key_final != NULL)
            {
                key_final->key_type = key_final_type;
                key_final->symbs_count = closure->attrib_set_count;
                key_final->symbs = malloc(key_final->symbs_count * sizeof(symb_id_kt));
                memcpy(key_final->symbs, closure->attrib_set, closure->attrib_set_count * sizeof(symb_id_kt));
            }
        }

        // Save the desired keys at the provided list.
        keys->key_count += key_types & KEY_PRIMARY ? keys_primary.key_count : 0;
        keys->key_count += key_types & KEY_SUPER ? keys_primary.key_count : 0;
        keys->keys = malloc(keys->key_count * sizeof(candidate_key_st));
        keys->key_count = 0; // It will be set to the desired value again in if statements below.
        if (key_types & KEY_PRIMARY)
        {
            memcpy(&keys->keys[keys->key_count], keys_primary.keys, keys_primary.key_count * sizeof(candidate_key_st));
            keys->key_count += keys_primary.key_count;
        }
        if (key_types & KEY_SUPER)
        {
            memcpy(&keys->keys[keys->key_count], keys_super.keys, keys_super.key_count * sizeof(candidate_key_st));
            keys->key_count += keys_super.key_count;
        }

        free(key_tmp.symbs);
        free(keys_super.keys);
        free(keys_primary.keys);
    }
    return 0;
}