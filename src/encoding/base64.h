/*
 *                           Copyright (C) 2005-2020 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef TULIP_ENCODING_BASE64_H
#define TULIP_ENCODING_BASE64_H 1

#include <stdlib.h>

unsigned char *base64_encode_buffer(const unsigned char *buffer, const size_t buffer_size, size_t *out_size);

#endif
