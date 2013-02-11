#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <stdlib.h>

/*
 * This small C code creates an initrd image with the following structure.
 *
 * o In the first 4 bytes there are written the number of files present in the
 *   initrd output file.
 * o After this all the header structures (struct header) are written for each
 *   file.
 * o Then all the file contents is written
 */

/*
 * Main structure to understand the file
 */
struct header {
	char name[32];        /* File name */
	unsigned int length;  /* File length */
	unsigned int offset;  /* Offset where the file is located starting from
			       * the beginning of the file */
};

/* Maximum size for one file */
#define MAX    (0x400000)

char buffer[MAX];
int main(int argc, char *argv[])
{
	FILE *initrd, *f_tmp;
	const int num_files = argc - 1;
	int i, len;
	struct header h_tmp;

	int off = (sizeof(struct header) * num_files) + sizeof(int);

	if (argc == 1) {
		printf("Usage: %s <files>\n", argv[0]);
		return 1;
	}

	initrd = fopen("initrd.img", "w");

	/* Write the number of files */
	fwrite(&num_files, sizeof(int), 1, initrd);

	/* Write all the headers */
	for (i = 0 ; i < num_files ; i++) {
		f_tmp = fopen(argv[1 + i], "r");
    if (f_tmp == NULL) {
      fprintf (stderr, "File %s not found!\n", argv[1 + i]);
      exit (1);
    }

		fseek(f_tmp, 0L, SEEK_END);
		len = ftell(f_tmp);
		if (len > MAX) {
			printf("File %s is bigger than %d\n", argv[1 + i], MAX);
      exit (1);
		}

		strcpy(h_tmp.name, basename(argv[1 + i]));
		h_tmp.length = len;
		h_tmp.offset = off;
		off += h_tmp.length;

		fwrite(&h_tmp, sizeof(struct header), 1, initrd);

		fclose(f_tmp);
	}

	/* Write all the files */
	for (i = 0 ; i < num_files ; i++) {
		f_tmp = fopen(argv[1 + i], "r");

		fseek(f_tmp, 0L, SEEK_END);
		len = ftell(f_tmp);

		rewind(f_tmp);

		fread(&buffer, 1, len, f_tmp);

		fwrite(&buffer, len, 1, initrd);

		fclose(f_tmp);
	}

	fclose(initrd);

	return 0;
}
