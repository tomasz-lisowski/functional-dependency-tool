#pragma once

#include "common.h"

/**
 * @brief Lookup an ID of a symbol.
 * @param symb What symbol to lookup.
 * @param attr_dict Dictionary to use for lookup.
 * @return ID of @p symb.
 */
symb_id_kt symb_to_id(symb_kt symb, attrib_dict_st *attr_dict);

/**
 * @brief Looks up what symbol has a given ID.
 * @param symb_id ID to lookup.
 * @param attr_dict Dictionary to use for lookup.
 * @return Symbol identified by @p id .
 */
symb_kt id_to_symb(symb_id_kt symb_id, attrib_dict_st *attr_dict);