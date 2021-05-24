#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>

#include "common.h"

#define FUNC_DEP_BUF_SIZE 1024
#define FUNC_DEPS_MAX 255
#define SYMBS_MAX 255

/**
 * @brief Lookup an ID of a symbol.
 * @param symb What symbol to lookup.
 * @param attr_dict Dictionary to use for lookup.
 * @return ID of @p symb.
 */
static symb_id_kt symb_to_id(symb_kt symb, attrib_dict_st *attr_dict)
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
    printf("Could not find ID of symbol '%c'.", symb);
    exit(1);
}

/**
 * @brief Looks up what symbol has a given ID.
 * @param symb_id ID to lookup.
 * @param attr_dict Dictionary to use for lookup.
 * @return Symbol identified by @p id .
 */
static symb_kt id_to_symb(symb_id_kt symb_id, attrib_dict_st *attr_dict)
{
    assert(attr_dict != NULL);
    assert(attr_dict->symbs != NULL);

    if (symb_id < attr_dict->symbs_len)
    {
        return attr_dict->symbs[symb_id];
    }
    printf("Could not find symbol for ID '%u'.", symb_id);
    exit(1);
}

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
 * @brief Print out all functional dependencies in a list in a specified format.
 * @param format The desired format of the output (0 = {A} -> {B}, 1 = A-->B, 2 = A -> B, default is verbose).
 * @param func_deps_info What functional dependencies to print.
 */
static void print_func_deps_formatted(uint8_t format, func_dep_info_st *func_deps_info)
{
    assert(func_deps_info != NULL);
    assert(func_deps_info->func_deps != NULL);
    assert(func_deps_info->attrib_dict != NULL);

    attrib_dict_st *attrib_dict = func_deps_info->attrib_dict;

    for (uint32_t fd_idx = 0; fd_idx < func_deps_info->func_deps_count; fd_idx += 1)
    {
        func_dep_st *fd = &func_deps_info->func_deps[fd_idx];

        if (format == 0)
        {
            printf("{");
            for (uint32_t lhs_idx = 0; lhs_idx < fd->lhs_len; lhs_idx += 1)
            {
                printf("%c", id_to_symb(fd->lhs[lhs_idx], attrib_dict));
                if (lhs_idx + 1 < fd->lhs_len)
                {
                    printf(",");
                }
            }
            printf("} -> {");
            for (uint32_t rhs_idx = 0; rhs_idx < fd->rhs_len; rhs_idx += 1)
            {
                printf("%c", id_to_symb(fd->rhs[rhs_idx], attrib_dict));
                if (rhs_idx + 1 < fd->rhs_len)
                {
                    printf(",");
                }
            }
            printf("},");
        }
        else if (format == 1)
        {
            for (uint32_t lhs_idx = 0; lhs_idx < fd->lhs_len; lhs_idx += 1)
            {
                printf("%c", id_to_symb(fd->lhs[lhs_idx], attrib_dict));
                if (lhs_idx + 1 < fd->lhs_len)
                {
                    printf(",");
                }
            }
            printf("-->");
            for (uint32_t rhs_idx = 0; rhs_idx < fd->rhs_len; rhs_idx += 1)
            {
                printf("%c", id_to_symb(fd->rhs[rhs_idx], attrib_dict));
                if (rhs_idx + 1 < fd->rhs_len)
                {
                    printf(",");
                }
            }
            printf(";");
        }
        else if (format == 2)
        {
            for (uint32_t lhs_idx = 0; lhs_idx < fd->lhs_len; lhs_idx += 1)
            {
                printf("%c", id_to_symb(fd->lhs[lhs_idx], attrib_dict));
                if (lhs_idx + 1 < fd->lhs_len)
                {
                    printf(",");
                }
            }
            printf(" -> ");
            for (uint32_t rhs_idx = 0; rhs_idx < fd->rhs_len; rhs_idx += 1)
            {
                printf("%c", id_to_symb(fd->rhs[rhs_idx], attrib_dict));
                if (rhs_idx + 1 < fd->rhs_len)
                {
                    printf(",");
                }
            }
        }
        else
        {
            printf("FD %u (#LHS: %u, #RHS: %u)\n", fd_idx, fd->lhs_len, fd->rhs_len);
            for (uint32_t lhs_idx = 0; lhs_idx < fd->lhs_len; lhs_idx += 1)
            {
                printf("%c ", id_to_symb(fd->lhs[lhs_idx], attrib_dict));
            }

            printf("-> ");
            for (uint32_t rhs_idx = 0; rhs_idx < fd->rhs_len; rhs_idx += 1)
            {
                printf("%c ", id_to_symb(fd->rhs[rhs_idx], attrib_dict));
            }
            printf("\n\n");
        }
        printf("\n");
    }
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

    if (argc == 2 && argv[1][0] == 'b')
    {
        print_func_deps_formatted(1, &func_deps_info);
    }
    else if (argc == 2 && argv[1][0] == 'g')
    {
        print_func_deps_formatted(0, &func_deps_info);
    }
    else
    {
        print_func_deps_formatted(2, &func_deps_info);
    }
}
