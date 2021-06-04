#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "func_dep.h"
#include "utils.h"

void print_func_dep(uint8_t format, func_dep_st *func_dep, attrib_dict_st *attrib_dict)
{
    switch (format)
    {
    case 0:
        printf("{");
        break;
    case 1:
        break;
    case 2:
        break;
    default:
        printf("FD (#LHS: %u, #RHS: %u)\n", func_dep->lhs_len, func_dep->rhs_len);
        break;
    }

    for (uint32_t lhs_idx = 0; lhs_idx < func_dep->lhs_len; lhs_idx += 1)
    {
        switch (format)
        {
        case 0:
            printf("%c", id_to_symb(func_dep->lhs[lhs_idx], attrib_dict));
            if (lhs_idx + 1 < func_dep->lhs_len)
            {
                printf(",");
            }
            break;
        case 1:
            printf("%c", id_to_symb(func_dep->lhs[lhs_idx], attrib_dict));
            if (lhs_idx + 1 < func_dep->lhs_len)
            {
                printf(",");
            }
            break;
        case 2:
            printf("%c", id_to_symb(func_dep->lhs[lhs_idx], attrib_dict));
            if (lhs_idx + 1 < func_dep->lhs_len)
            {
                printf(",");
            }
            break;
        default:
            printf("%c ", id_to_symb(func_dep->lhs[lhs_idx], attrib_dict));
            break;
        }
    }

    switch (format)
    {
    case 0:
        printf("} -> {");
        break;
    case 1:
        printf("-->");
        break;
    case 2:
        printf(" -> ");
        break;
    default:
        printf("-> ");
        break;
    }

    for (uint32_t rhs_idx = 0; rhs_idx < func_dep->rhs_len; rhs_idx += 1)
    {
        switch (format)
        {
        case 0:
            printf("%c", id_to_symb(func_dep->rhs[rhs_idx], attrib_dict));
            if (rhs_idx + 1 < func_dep->rhs_len)
            {
                printf(",");
            }
            break;
        case 1:
            printf("%c", id_to_symb(func_dep->rhs[rhs_idx], attrib_dict));
            if (rhs_idx + 1 < func_dep->rhs_len)
            {
                printf(",");
            }
            break;
        case 2:
            printf("%c", id_to_symb(func_dep->rhs[rhs_idx], attrib_dict));
            if (rhs_idx + 1 < func_dep->rhs_len)
            {
                printf(",");
            }
            break;
        default:
            printf("%c ", id_to_symb(func_dep->rhs[rhs_idx], attrib_dict));
            break;
        }
    }

    switch (format)
    {
    case 0:
        printf("},");
        break;
    case 1:
        printf(";");
        break;
    case 2:
        break;
    default:
        printf("\n");
        break;
    }
    printf("\n");
}

void print_func_deps(uint8_t format, func_dep_info_st *func_deps_info)
{
    assert(func_deps_info != NULL);
    assert(func_deps_info->func_deps != NULL);
    assert(func_deps_info->attrib_dict != NULL);

    for (uint32_t fd_idx = 0; fd_idx < func_deps_info->func_deps_count; fd_idx += 1)
    {
        func_dep_st *fd = &func_deps_info->func_deps[fd_idx];
        print_func_dep(format, fd, func_deps_info->attrib_dict);
    }
}

symb_id_kt symb_to_id(symb_kt symb, attrib_dict_st *attr_dict)
{
    assert(attr_dict != NULL);
    assert(attr_dict->symbs != NULL);

    for (uint32_t idx = 0; idx < attr_dict->symbs_len; idx += 1)
    {
        if (attr_dict->symbs[idx] == symb)
        {
            return idx;
        }
    }
    printf("Could not find ID of symbol '%c'\n", symb);
    exit(1);
}

symb_kt id_to_symb(symb_id_kt symb_id, attrib_dict_st *attr_dict)
{
    assert(attr_dict != NULL);
    assert(attr_dict->symbs != NULL);

    if (symb_id < attr_dict->symbs_len)
    {
        return attr_dict->symbs[symb_id];
    }
    printf("Could not find symbol for ID '%u'\n", symb_id);
    exit(1);
}

uint8_t comp_symb_id(void *const a, void *const b)
{
    int32_t const dif = *((symb_id_kt *)a) - *((symb_id_kt *)b);
    uint8_t ret = 0;
    if (dif == 0)
    {
        ret += COMP_EQUAL;
    }
    if (dif > 0)
    {
        ret += COMP_GREATER;
    }
    return ret;
}
