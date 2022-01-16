#pragma once

#include <stdint.h>
#include <stddef.h>

#ifndef SHOMP_NO_COMPRESSION
size_t shomp_uleb_encode(uint8_t *out, size_t num);
size_t shomp_compress(uint8_t *out, const uint8_t *data, size_t sz);
#endif

size_t shomp_uleb_decode(const uint8_t *data, size_t *dataI);
size_t shomp_decompress(uint8_t *out, const uint8_t *compressed, size_t sz);
