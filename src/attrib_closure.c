#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "attrib_closure.h"
#include "utils.h"

/**
 * @brief Check if a symbol is contained in the closure.
 * @param closure The closure to check.
 * @param symb_id Symbol to check for.
 * @return `false' if not contained, `true' if contained.
 */
static bool closure_map_contains(attrib_closure_st *closure, symb_id_kt symb_id)
{
    for (uint32_t map_idx = 0; map_idx < closure->closure_len; map_idx += 1)
    {
        if (closure->closure[map_idx] == symb_id)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Check if the left hand side (LHS) of a functional dependency contains only attributes that are also present in
 * the closure.
 * @param closure closure that is checked for LHS attributes.
 * @param fd Functional dependency to check LHS of.
 * @return `false' if LHS contains attributes that are not in the closure, `true' otherwise (LHS satisfied).
 */
static bool fd_lhs_satisfied(attrib_closure_st *closure, func_dep_st *fd)
{
    for (uint32_t lhs_idx = 0; lhs_idx < fd->lhs_len; lhs_idx += 1)
    {
        bool lhs_attrib_satisfied = false;
        if (closure_map_contains(closure, fd->lhs[lhs_idx]))
        {
            lhs_attrib_satisfied = true;
        }
        if (lhs_attrib_satisfied == false)
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief Add a symbol to the closure making sure that no duplicates get added.
 * @param closure closure to add symbol to.
 * @param symb_id Symbol to add to closure.
 * @return `false' if symbol was a duplicate hence closure remains unchanged, `true' if symbol was added.
 */
static bool closure_map_add(attrib_closure_st *closure, symb_id_kt symb_id)
{
    if (closure_map_contains(closure, symb_id))
    {
        return false; // Already contained in the closure.
    }
    closure->closure[closure->closure_len++] = symb_id;
    return true;
}

void print_closure(attrib_closure_st *closure, attrib_dict_st *attrib_dict)
{
    printf("{");
    for (uint32_t attrib_idx = 0; attrib_idx < closure->attrib_set_count; attrib_idx += 1)
    {
        char symb = 'A';
        if (attrib_dict == NULL)
        {
            if (closure->attrib_set[attrib_idx] > 25)
            {
                symb = '?';
            }
            else
            {
                symb += closure->attrib_set[attrib_idx];
            }
        }
        else
        {
            symb = id_to_symb(closure->attrib_set[attrib_idx], attrib_dict);
        }
        printf("%c", symb);
        if (attrib_idx + 1 < closure->attrib_set_count)
        {
            printf(" ,");
        }
    }
    printf("}+ = ");
    printf("{");
    for (uint32_t closure_attr_idx = 0; closure_attr_idx < closure->closure_len; closure_attr_idx += 1)
    {
        char symb = 'A';
        if (attrib_dict == NULL)
        {
            if (closure->closure[closure_attr_idx] > 25)
            {
                symb = '?';
            }
            else
            {
                symb += closure->closure[closure_attr_idx];
            }
        }
        else
        {
            symb = id_to_symb(closure->closure[closure_attr_idx], attrib_dict);
        }
        printf("%c", symb);
        if (closure_attr_idx + 1 < closure->closure_len)
        {
            printf(" ,");
        }
    }
    printf("}\n");
}

void print_closure_arr(attrib_closure_arr_st *closure_arr, attrib_dict_st *attrib_dict)
{
    for (uint32_t closure_idx = 0; closure_idx < closure_arr->closures_count; closure_idx += 1)
    {
        print_closure(&closure_arr->closures[closure_idx], attrib_dict);
    }
}

uint32_t attrib_closure_compute(attrib_closure_st *closure, func_dep_info_st *func_deps_info)
{
    assert(closure != NULL);
    assert((closure->attrib_set != NULL) ||
           (closure->attrib_set == NULL &&
            closure->attrib_set_count == 0)); // It is possible to get closure for an empty set.
    assert(closure->closure == NULL);
    assert(closure->closure_len == 0);
    assert(func_deps_info != NULL);
    assert(func_deps_info->func_deps != NULL);
    assert(func_deps_info->attrib_dict != NULL);

    // Empty attribute set means empty closure.
    if (closure->attrib_set_count == 0)
    {
        closure->closure_len = 0;
        return 0;
    }

    // TODO: Maybe this should be sized `just right' instead of a pre-allocating space for all symbols.
    closure->closure = calloc(sizeof(symb_id_kt), func_deps_info->attrib_dict->symbs_len);
    closure->closure_len = 0;

    // The attribute set is contained in its own closure.
    for (uint32_t attrib_set_idx = 0; attrib_set_idx < closure->attrib_set_count; attrib_set_idx += 1)
    {
        closure_map_add(closure, closure->attrib_set[attrib_set_idx]);
    }

    // Find closure.
    {
        bool closure_changed = false;
        func_dep_st *fd;
        while (1)
        {
            for (uint32_t fd_idx = 0; fd_idx < func_deps_info->func_deps_count; fd_idx += 1)
            {
                fd = &func_deps_info->func_deps[fd_idx];
                if (fd_lhs_satisfied(closure, fd))
                {
                    for (uint32_t rhs_idx = 0; rhs_idx < fd->rhs_len; rhs_idx += 1)
                    {
                        closure_changed |= closure_map_add(closure, fd->rhs[rhs_idx]);
                    }
                }
            }

            // Loop will continue an iteration over the entire FD list does not lead to a change of the closure.
            if (!closure_changed)
            {
                break;
            }
            else
            {
                closure_changed = false;
            }
        }
    }

    // Sort the closure set.
    sort(closure->closure, closure->closure_len, sizeof(symb_id_kt), comp_symb_id);
    flip(closure->closure, closure->closure_len, sizeof(symb_id_kt));
    return 0;
}
