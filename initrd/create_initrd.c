#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <stdlib.h>
#include <stdint.h>

/*
 * This small C code creates an initrd image with the following structure.
 *
 * o In the first 4 bytes there are written the number of files present in the
 *   initrd output file.
 * o After this all the header structures (struct header) are written for each
 *   file.
 * o Then all the file contents is written
 */

#define TYPE_FILE 1
#define TYPE_CHAR 2
/*
 * Main structure to understand the file
 *
 * XXX:
 *    If you change the following structure, make sure you update the structure
 *    in file vms/fs/main.c
 *    These two MUST be exactly identical
 */
struct header {
	char name[32];        /* File name */
	uint32_t type;    /* Type of the file (normal file, char, block ...) */
	uint32_t length;  /* File length */
	uint64_t dst;     /* Cpuid of the destination (for CHAR or BLOCK fds) */
	uint64_t offset;  /* Offset where the file is located starting from
			   * the beginning of the file */
};

/* Maximum size for one file */
#define MAX    (0x400000)

char buffer[MAX];
int main(int argc, char *argv[])
{
	FILE *initrd, *f_tmp;
	const int normal_files = argc - 1;
	const int char_files = 2;
	const int total_files = normal_files + char_files;
	int i, len;
	struct header h_tmp;

	int off = (sizeof(struct header) * total_files) + sizeof(int);

	if (argc == 1) {
		printf("Usage: %s <files>\n", argv[0]);
		return 1;
	}

	initrd = fopen("initrd.img", "w");

	/* Write the number of files */
	fwrite(&total_files, sizeof(int), 1, initrd);

	/* Char files: stdin, stdout */
	strcpy(h_tmp.name, "stdin");
	h_tmp.type = TYPE_CHAR;
	h_tmp.length = 0;
	h_tmp.dst = 0;
	h_tmp.offset = 0;
	fwrite(&h_tmp, sizeof(struct header), 1, initrd);
	strcpy(h_tmp.name, "stdout");
	h_tmp.type = TYPE_CHAR;
	h_tmp.length = 0;
	h_tmp.dst = 0;
	h_tmp.offset = 0;
	fwrite(&h_tmp, sizeof(struct header), 1, initrd);

	/* Write all the headers */
	for (i = 0 ; i < normal_files ; i++) {
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
		h_tmp.type = TYPE_FILE;
		h_tmp.length = len;
		h_tmp.dst = 0;
		h_tmp.offset = off;
		off += h_tmp.length;

		fwrite(&h_tmp, sizeof(struct header), 1, initrd);

		fclose(f_tmp);
	}

	/* Write all the files */
	for (i = 0 ; i < normal_files ; i++) {
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
