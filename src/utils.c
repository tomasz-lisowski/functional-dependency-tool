#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

bool subset_sorted(void *set_a, void *set_b, uint32_t set_a_len, uint32_t set_b_len, uint32_t el_size)
{
    uint32_t set_a_idx = 0;
    for (uint32_t set_b_idx = 0; set_b_idx < set_b_len; set_b_idx += 1)
    {
        if (memcmp(&((uint8_t *)set_b)[set_b_idx * el_size], &((uint8_t *)set_a)[set_a_idx * el_size], el_size) == 0)
        {
            set_a_idx += 1;
        }
    }
    if (set_a_len == set_a_idx)
    {
        return true;
    }
    return false;
}

void sort(void *data, uint32_t el_count, uint32_t el_size, uint8_t (*comparator)(void *const, void *const))
{
    // Insertion sort.
    for (uint32_t i = 1; i < el_count; i++)
    {
        for (uint32_t j = i;
             j > 0 &&
             (comparator(&((uint8_t *)data)[j * el_size], &((uint8_t *)data)[(j - 1) * el_size]) ^ COMP_GREATER) == 0;
             j--)
        {
            // Swapping [j] and [j-1]
            uint8_t el_to_swap[el_size];
            memcpy(&el_to_swap, &((uint8_t *)data)[j * el_size], el_size);
            memcpy(&((uint8_t *)data)[j * el_size], &((uint8_t *)data)[(j - 1) * el_size], el_size);
            memcpy(&((uint8_t *)data)[(j - 1) * el_size], &el_to_swap, el_size);
        }
    }
}

void flip(void *data, uint32_t el_count, uint32_t el_size)
{
    uint32_t count_half = el_count % 2 == 0 ? el_count / 2 : (el_count - 1) / 2;
    for (uint32_t data_idx = 0; data_idx < count_half; data_idx += 1)
    {
        uint8_t *el_a = &((uint8_t *)data)[data_idx * el_size];
        uint8_t *el_b = &((uint8_t *)data)[(el_count - 1 - data_idx) * el_size];
        uint8_t tmp[el_size];
        memcpy(tmp, el_a, el_size);
        memcpy(el_a, el_b, el_size);
        memcpy(el_b, tmp, el_size);
    }
}
