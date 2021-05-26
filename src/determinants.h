#pragma once

#include "attrib_closure.h"
#include "candidate_keys.h"
#include <stdint.h>

typedef struct determinant_info_s
{
    symb_id_kt *attrib_set;
    uint32_t attrib_set_count;
    attrib_closure_arr_st *determinants;
} determinant_info_st;

uint32_t determinants_compute(attrib_closure_arr_st *closures_all, determinant_info_st *det_info);
