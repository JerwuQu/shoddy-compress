#include "shomp.h"

#ifndef SHOMP_NO_COMPRESSION
size_t shomp_uleb_encode(uint8_t *out, size_t num)
{
	size_t len = 0;
	do {
		const uint8_t low = num & 0x7f;
		num >>= 7;
		if (out) {
			out[len] = num ? (low | 0x80) : low;
		}
		len++;
	} while (num);
	return len;
}

size_t shomp_compress(uint8_t *out, const uint8_t *data, size_t sz)
{
	// NOTE: the name doesn't lie, this is shoddy
	// Basically a variation of LZ77 using an infinite window with ULEB128 for distances
	// Each 8 segments are prefixed by a byte telling what they are
	size_t chunk = 8, prefixI = 0, outI = 0;
	for (size_t i = 0; i < sz; i++) {
		size_t longestMatchDist, longestMatchLen = 0;
		if (chunk == 8) {
			chunk = 0;
			prefixI = outI++;
			if (out) {
				out[prefixI] = 0;
			}
		}
		for (size_t j = 0; j < i; j++) {
			size_t matchLen = 0;
			while (j + matchLen < i
					&& i + matchLen < sz
					&& data[i + matchLen] == data[j + matchLen]) {
				matchLen++;
			}
			if (matchLen > longestMatchLen) {
				longestMatchLen = matchLen;
				longestMatchDist = i - j;
			}
		}
		if (longestMatchLen) {
			const size_t ulebLen =
				shomp_uleb_encode(NULL, longestMatchDist - 1)
				+ shomp_uleb_encode(NULL, longestMatchLen - 1);
			if (ulebLen > longestMatchLen * 2) {
				if (out) {
					out[outI] = data[i];
				}
				outI++;
			} else {
				if (out) {
					out[prefixI] |= 1 << chunk;
					outI += shomp_uleb_encode(out + outI, longestMatchDist - 1);
					outI += shomp_uleb_encode(out + outI, longestMatchLen - 1);
				} else {
					outI += ulebLen;
				}
				i += longestMatchLen - 1;
			}
		} else {
			if (out) {
				out[outI] = data[i];
			}
			outI++;
		}
		chunk++;
	}
	return outI;
}
#endif // SHOMP_NO_COMPRESSION

size_t shomp_uleb_decode(const uint8_t *data, size_t *dataI)
{
	size_t result = 0, shift = 0;
	while (1) {
		const uint8_t byte = data[*dataI];
		(*dataI)++;
		result |= (byte & 0x7f) << shift;
		if (!(byte & 0x80)) {
			break;
		}
		shift += 7;
	}
	return result;
}

size_t shomp_decompress(uint8_t *out, const uint8_t *compressed, size_t sz)
{
	size_t i = 0, len = 0, shift = 0, prefix = 0;
	while (i < sz) {
		if (!shift) {
			prefix = compressed[i++];
			shift = 8;
		}
		shift--;
		if (prefix & (0x80 >> shift)) {
			const size_t cdist = shomp_uleb_decode(compressed, &i) + 1;
			const size_t clen = shomp_uleb_decode(compressed, &i) + 1;
			if (out) {
				size_t c = clen;
				do {
					c--;
					out[len + c] = out[len - cdist + c];
				} while (c);
			}
			len += clen;
		} else {
			if (out) {
				out[len] = compressed[i];
			}
			i++;
			len++;
		}
	}
	return len;
}
