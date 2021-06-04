#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "determinant.h"
#include "utils.h"

uint32_t determinants_compute(attrib_closure_arr_st *closures_all, determinant_info_st *det_info)
{
    assert(closures_all != NULL);
    assert(closures_all->closures != NULL);
    assert(closures_all->closures_count != 0);
    assert(det_info != NULL);
    assert(det_info->determinants != NULL);
    assert((det_info->attrib_set != NULL) || (det_info->attrib_set == NULL && det_info->attrib_set_count == 0));
    assert(det_info->determinants->closures == NULL);
    assert(det_info->determinants->closures_count == 0);

    if (det_info->attrib_set_count == 0 || closures_all->closures_count == 0)
    {
        // This is already ensured with the asserts above.
        // det_info->determinants->closures_count = 0;
        return 0;
    }

    attrib_closure_arr_st det_info_tmp = {0};
    det_info_tmp.closures = calloc(closures_all->closures_count, sizeof(attrib_closure_st));
    det_info_tmp.closures_count = 0;

    // Find all closures that contain at least the requested attributes and which are minimal.
    for (uint32_t closure_idx = 0; closure_idx < closures_all->closures_count; closure_idx += 1)
    {
        attrib_closure_st *closure = &closures_all->closures[closure_idx];
        // Only interested in covers that contain all requested attributes but where LHS of the closure does not contain
        // those attributes.
        if (subset_sorted(det_info->attrib_set, closure->closure, det_info->attrib_set_count, closure->closure_len,
                          sizeof(symb_id_kt)) &&
            !subset_sorted(det_info->attrib_set, closure->attrib_set, det_info->attrib_set_count,
                           closure->attrib_set_count, sizeof(symb_id_kt)))
        {
            // Check if LHS is minimal.
            bool not_minimal = false;
            for (uint32_t closure_minimal_idx = 0; closure_minimal_idx < det_info_tmp.closures_count;
                 closure_minimal_idx += 1)
            {
                not_minimal |= subset_sorted(det_info_tmp.closures[closure_minimal_idx].attrib_set, closure->attrib_set,
                                             det_info_tmp.closures[closure_minimal_idx].attrib_set_count,
                                             closure->attrib_set_count, sizeof(symb_id_kt));
            }
            if (!not_minimal)
            {
                // Shallow copy to a temporary list.
                memcpy(&det_info_tmp.closures[det_info_tmp.closures_count++], closure, sizeof(attrib_closure_st));
            }
        }
    }

    // Copy all closures from the temporary list to the user provided one.
    det_info->determinants->closures_count = det_info_tmp.closures_count;
    det_info->determinants->closures = malloc(det_info_tmp.closures_count * sizeof(attrib_closure_st));
    for (uint32_t closure_idx = 0; closure_idx < det_info_tmp.closures_count; closure_idx += 1)
    {
        attrib_closure_st *closure_dest = &det_info->determinants->closures[closure_idx];
        attrib_closure_st *closure_src = &det_info_tmp.closures[closure_idx];

        // Deep copy the closure to the determinant info.
        closure_dest->attrib_set = malloc(closure_src->attrib_set_count * sizeof(symb_id_kt));
        closure_dest->attrib_set_count = closure_src->attrib_set_count;
        memcpy(closure_dest->attrib_set, closure_src->attrib_set, closure_src->attrib_set_count * sizeof(symb_id_kt));

        closure_dest->closure = malloc(closure_src->closure_len * sizeof(symb_id_kt));
        closure_dest->closure_len = closure_src->closure_len;
        memcpy(closure_dest->closure, closure_src->closure, closure_src->closure_len * sizeof(symb_id_kt));
    }

    free(det_info_tmp.closures);
    return 0;
}
