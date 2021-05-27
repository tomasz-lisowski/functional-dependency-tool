#pragma once

#include <stdbool.h>

#include "candidate_keys.h"
#include "common.h"

bool check_bcnf(func_dep_info_st *func_deps_info, candidate_keys_st *keys_primary);
bool check_3nf(func_dep_info_st *func_deps_info, candidate_keys_st *keys_primary);
