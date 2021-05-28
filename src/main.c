#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "candidate_keys.h"
#include "common.h"
#include "determinants.h"
#include "norm.h"
#include "utils.h"

#define FUNC_DEP_BUF_SIZE 1024
#define FUNC_DEPS_MAX 255
#define SYMBS_MAX 255

/**
 * @brief Read the input file into a buffer,
 * @param buffer_loc Where to store pointer to buffer where we write the file contents.
 * @param file_len Where to write the length of the file that was read.
 * @return 0 on success, > 0 on failure.
 */
static uint32_t input_read(char **buffer_loc, uint32_t *file_len)
{
    assert(buffer_loc != NULL);
    assert(file_len != NULL);

    FILE *f = fopen("fd.txt", "rb");
    fseek(f, 0, SEEK_END);
    *file_len = ftell(f);
    fseek(f, 0, SEEK_SET);

    *buffer_loc = malloc(*file_len + 1);
    fread(*buffer_loc, 1, *file_len, f);
    fclose(f);
    return 0;
}

/**
 * @brief Given the contents of a file containing a description of functional dependencies, parse it and save output to
 * a struct.
 * @param input_raw Raw description of the functional dependencies.
 * @param input_len How long @p input_raw is.
 * @param func_deps_info Where to save the parsed data.
 */
static void input_parse(char *input_raw, uint32_t input_len, func_dep_info_st *func_deps_info)
{
    assert(input_raw != NULL);
    assert(func_deps_info != NULL);
    assert(func_deps_info->func_deps != NULL);
    assert(func_deps_info->attrib_dict != NULL);

    attrib_dict_st *attrib_dict = func_deps_info->attrib_dict;
    uint32_t row_idx = 0;
    uint32_t fd_idx = 0;
    uint8_t arrow_seen = false;

    for (uint32_t raw_idx = 0; raw_idx < input_len; raw_idx += 1)
    {
        char ch = input_raw[raw_idx];
        switch (ch)
        {
        case '-':
            if (raw_idx + 1 < input_len && input_raw[raw_idx + 1] == '>')
            {
                raw_idx += 1;
                arrow_seen = true;
                continue;
            }
        case ',':
        case ' ':
        case '{':
        case '}':
            continue;
        case '\n':
            func_deps_info->func_deps_count += 1;
            arrow_seen = false;
            if (row_idx > 0)
            {
                fd_idx += 1;
            }
            row_idx += 1;
            func_dep_st *fd = &func_deps_info->func_deps[fd_idx];
            fd->lhs = calloc(1, FUNC_DEP_BUF_SIZE);
            fd->lhs_len = 0;
            fd->rhs = calloc(1, FUNC_DEP_BUF_SIZE);
            fd->rhs_len = 0;
            continue;
        default:
            break;
        }

        if (row_idx == 0)
        {
            attrib_dict->symbs[attrib_dict->symbs_len++] = ch;
        }
        else
        {
            func_dep_st *fd = &func_deps_info->func_deps[fd_idx];
            if (arrow_seen)
            {
                fd->rhs[fd->rhs_len++] = symb_to_id(ch, attrib_dict);
            }
            else
            {
                fd->lhs[fd->lhs_len++] = symb_to_id(ch, attrib_dict);
            }
        }
    }
}

int32_t main(int32_t argc, char *argv[argc])
{
    // State
    attrib_dict_st attrib_dict = {0};
    func_dep_info_st func_deps_info = {0};
    func_dep_st func_deps[FUNC_DEPS_MAX] = {0};
    func_deps_info.func_deps = func_deps;
    func_deps_info.func_deps_count = 0;
    func_deps_info.attrib_dict = &attrib_dict;

    // TODO: Do this dynamically.
    attrib_dict.symbs = calloc(1, SYMBS_MAX);

    char *input_raw;
    uint32_t input_len = 0;
    input_read(&input_raw, &input_len);
    input_parse(input_raw, input_len, &func_deps_info);

    if (argc == 3 && strcmp(argv[1], "--fd-list") == 0 && strlen(argv[2]) == 1)
    {
        switch (argv[2][0])
        {
        case 'r':
            print_func_deps(0, &func_deps_info);
            break;
        case 'c':
            print_func_deps(1, &func_deps_info);
            break;
        case 's':
            print_func_deps(2, &func_deps_info);
            break;
        default:
            print_func_deps(UINT8_MAX, &func_deps_info);
            break;
        }
    }
    else if (argc == 3 && strcmp(argv[1], "--det") == 0)
    {
        uint32_t attrib_set_len = strlen(argv[2]);

        candidate_keys_st keys = {0};
        attrib_closure_arr_st closures_all = {0};
        keys_compute(&keys, &closures_all, &func_deps_info, 0);

        symb_id_kt det_attrib_set[attrib_set_len];
        attrib_closure_arr_st det_closures = {.closures = NULL, .closures_count = 0};
        determinant_info_st det_info = {
            .attrib_set = det_attrib_set,
            .attrib_set_count = attrib_set_len,
            .determinants = &det_closures,
        };

        for (uint32_t attrib_idx = 0; attrib_idx < attrib_set_len; attrib_idx += 1)
        {
            det_attrib_set[attrib_idx] = symb_to_id(argv[2][attrib_idx], &attrib_dict);
        }
        determinants_compute(&closures_all, &det_info);
        print_closure_arr(det_info.determinants, &attrib_dict);
    }
    else if (argc == 2 && strcmp(argv[1], "--closure-all") == 0)
    {
        candidate_keys_st keys = {0};
        attrib_closure_arr_st closures_all = {0};
        keys_compute(&keys, &closures_all, &func_deps_info, 0);
        print_closure_arr(&closures_all, &attrib_dict);
    }
    else if (argc == 3 && strcmp(argv[1], "--closure") == 0)
    {
        attrib_closure_st closure = {0};
        closure.attrib_set_count = strlen(argv[2]);
        closure.attrib_set = malloc(closure.attrib_set_count * sizeof(symb_id_kt));
        for (uint32_t attrib_idx = 0; attrib_idx < closure.attrib_set_count; attrib_idx += 1)
        {
            closure.attrib_set[attrib_idx] = symb_to_id(argv[2][attrib_idx], &attrib_dict);
        }
        attrib_closure_compute(&closure, &func_deps_info);
        print_closure(&closure, &attrib_dict);
    }
    else if (argc == 2 && strcmp(argv[1], "--norm") == 0)
    {
        candidate_keys_st keys_primary = {0};
        attrib_closure_arr_st closures_all = {0};
        keys_compute(&keys_primary, &closures_all, &func_deps_info, KEY_PRIMARY);
        if (check_bcnf(&func_deps_info, &keys_primary))
        {
            printf("BCNF: yes\n");
        }
        else
        {
            printf("BCNF: no\n");
        }
        if (check_3nf(&func_deps_info, &keys_primary))
        {
            printf("3NF: yes\n");
        }
        else
        {
            printf("3NF: no\n");
        }
    }
    else if (argc == 4 && strcmp(argv[1], "--keys") == 0 && strlen(argv[2]) == 1 && strlen(argv[3]) == 1)
    {
        key_type_et key_type = 0;
        switch (argv[2][0])
        {
        case 'p':
            key_type = KEY_PRIMARY;
            break;
        case 's':
            key_type = KEY_SUPER;
            break;
        case 'a':
        default:
            key_type = KEY_PRIMARY | KEY_SUPER;
            break;
        }

        candidate_keys_st keys = {0};
        attrib_closure_arr_st closures_all = {0};
        keys_compute(&keys, &closures_all, &func_deps_info, key_type);

        switch (argv[3][0])
        {
        case 'y':
            print_key_arr(&keys, &attrib_dict, 1);
            break;
        case 'n':
        default:
            print_key_arr(&keys, &attrib_dict, 0);
            break;
        }
    }
    else
    {
        printf("NOTE: The input file must always be located at './fd.txt'."
               "Usage: func_deps_tool <--fd-list format | --det attrib_set | --closure attrib_set | --closure-all | "
               "--keys types show_type | --norm>\n\n"
               "'--fd-list format': Print out the functional dependencies in some format. 's' for 'A,B -> C,D', 'c' "
               "for 'A,B-->C,D;', 'r' for '{A,B} -> {C,D},'.\n\n"
               "'--det attrib_set': Compute determinant of a set of attributes specified like e.g. 'ABCD' (these need "
               "to be sorted).\n\n"
               "'--closure attrib_set': Compute the closure of a set of attributes like e.g. 'ABCD' (these need to be "
               "sorted).\n\n"
               "'--closure-all': List all possible closures.\n\n"
               "'--keys types show_type': Compute keys where type can be 'p' for primary, 's' for super keys, 'a' for "
               "both primary and super, else all keys and non-keys get listed. 'show_type' can be set to 'y' (yes) or "
               "'n' (no) to tell program if the key type should be included in the listing.\n\n"
               "'--norm': Checks if the provided FDs are in BCNF and 3NF.\n\n");
    }
}
