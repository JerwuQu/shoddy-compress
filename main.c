#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "shomp.h"

int main(int argc, char **argv)
{
	if (argc != 4 || (argv[1][0] != 'c' && argv[1][0] != 'd')) {
		fprintf(stderr, "shomp <c/d> <in file> <out file>\n");
		return 1;
	}

	// Read file
	FILE *f = fopen(argv[2], "rb");
	assert(f);
	fseek(f, 0, SEEK_END);
	const size_t fsize = ftell(f);
	assert(fsize);
	fseek(f, 0, SEEK_SET);
	uint8_t *data = malloc(fsize);
	assert(data);
	assert(fread(data, fsize, 1, f) == 1);
	fclose(f);

	// Compress / Decompress
	size_t outsz;
	uint8_t *out;
	if (argv[1][0] == 'c') {
		outsz = shomp_compress(NULL, data, fsize);
		out = malloc(outsz);
		assert(out);
		shomp_compress(out, data, fsize);
		printf("%ld compressed to %ld\n", fsize, outsz);

		// Sanity check
		const size_t dsz = shomp_decompress(NULL, out, outsz);
		assert(dsz == fsize);
		uint8_t *decmp = malloc(dsz);
		assert(decmp);
		shomp_decompress(decmp, out, outsz);
		for (size_t i = 0; i < fsize; i++) {
			assert(data[i] == decmp[i]);
		}
		free(decmp);
	} else {
		outsz = shomp_decompress(NULL, data, fsize);
		out = malloc(outsz);
		assert(out);
		shomp_decompress(out, data, fsize);
		printf("%ld decompressed to %ld\n", fsize, outsz);
	}

	// Write file
	f = fopen(argv[3], "wb");
	assert(f);
	fwrite(out, outsz, 1, f);
	fclose(f);
	free(out);

	return 0;
}
