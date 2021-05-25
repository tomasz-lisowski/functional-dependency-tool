#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

/**
 * @brief Lookup an ID of a symbol.
 * @param symb What symbol to lookup.
 * @param attr_dict Dictionary to use for lookup.
 * @return ID of @p symb.
 */
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
    printf("Could not find ID of symbol '%c'.", symb);
    exit(1);
}

/**
 * @brief Looks up what symbol has a given ID.
 * @param symb_id ID to lookup.
 * @param attr_dict Dictionary to use for lookup.
 * @return Symbol identified by @p id .
 */
symb_kt id_to_symb(symb_id_kt symb_id, attrib_dict_st *attr_dict)
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