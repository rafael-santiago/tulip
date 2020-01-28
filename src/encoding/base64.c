/*
 *                           Copyright (C) 2005-2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <encoding/base64.h>
#include <base/memory.h>

static unsigned char g_base64_state[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
    'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
    'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
    'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
    's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2',
    '3', '4', '5', '6', '7', '8', '9', '+', '/'
};

#define base64_get_encoded_byte(b) ( (b) != '=' ? (b) : 0 )

unsigned char *base64_encode_buffer(const unsigned char *buffer, const size_t buffer_size, size_t *out_size) {
    const unsigned char *bp, *bp_end;
    unsigned char *out, *out_p;
    unsigned int block;
    size_t block_size, pad_size = 0;

    if (buffer == NULL || buffer_size == 0) {
        *out_size = 0;
        return NULL;
    }

    bp = buffer;
    bp_end = bp + buffer_size;

    *out_size = (buffer_size * 8) / 6;

    while ((*out_size % 4) != 0) {
        *out_size += 1;
    }

    out = (unsigned char *) getseg(*out_size);
    out_p = out;

    while (bp != bp_end) {

        block = 0;
        block_size = 0;

        while (bp != bp_end && block_size < 3) {
            block = block << 8 | *bp;
            block_size++;
            bp++;
        }

        pad_size = 3 - block_size;

        block = block << (pad_size << 3);

        *out_p       = g_base64_state[(block & 0x00FC0000) >> 18];
        *(out_p + 1) = g_base64_state[(block & 0x0003F000) >> 12];
        *(out_p + 2) = g_base64_state[(block & 0x00000FC0) >>  6];
        *(out_p + 3) = g_base64_state[block & 0x0000003F];

        out_p += 4 - pad_size;
    }

    while (pad_size > 0) {
        *out_p = '=';
        out_p++;
        pad_size--;
    }

    bp = NULL;
    bp_end = NULL;
    block = 0;

    return out;
}

#undef base64_get_encoded_byte
