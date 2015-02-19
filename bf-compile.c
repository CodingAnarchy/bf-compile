/*
BF-Compile.c
C program to compile brainfuck to assembly
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUCCESS 0

char *change_ext (const char* fname, char dot, char sep, const char* new) {
	char *retstr, *lastdot, *lastsep;

	// Error checks and allocate string
	if (fname == NULL)
		return NULL;
	if ((retstr = malloc(strlen(fname) + 4)) == NULL)
		return NULL;

	strcpy(retstr, fname);
	lastdot = strrchr(retstr, dot);
	lastsep = (sep == 0) ? NULL : strrchr(retstr, sep);

	// If it has an extension separator
	if (lastdot != NULL) {
		// and it is after the last separator
		if (lastsep != NULL) {
			if (lastsep < lastdot) {
				*lastdot = new;
			}
		} else {
			// Has extension with no path separator
			*lastdot = new;
		}
	}

	return retstr;

}

int compile_bf(const char *filename) {

	unsigned int ptr = 0;
	FILE *source;
	FILE *out;
	char* dest;
	char* obj;
	int c;
	int jmpId = 0;
	int *jmpStack;

	if ((source = fopen(filename, "rb")) == NULL) {
		fprintf(stderr, "Could not open file %s", filename);
		return -1;
	}
	
	dest = change_ext(filename, '.', '/', ".asm");

	if ((out = fopen(dest, "wb")) == NULL) {
		fprintf(stderr, "Could not create file %s", dest);
		return -1;
	}

	obj = change_ext(filename, '.', '/', ".o");

	// Header information in assembly file
	fputs("; ", out);
	fputs(dest, out);
	fputs("\n; compiled from ", out);
	fputs(filename, out);
	fputs("; assemble: nasm -f elf -o ", out);
	fputs(obj, out);
	fputs(" ", out);
	fputs(dest, out);
	fputs("\nlink: ld ", out);
	fputs(obj, out);
	fputs(" <cmd>\n\n", out);

	// Start of program in assembly file
	fputs("main:\n", out);
	fputs("  mov bh,0xa0\n", out);
	fputs("  mov di,bx\n", out);

	// Read in characters and write out assembly to handle them
	while ((c = fgetc(source)) != EOF) {
		switch(c) {
			case '>':
				fputs("; >\n  inc di\n", out);
				break;
			case '<': 
				fputs("; <\n  dec di\n", out);
				break;
			case '+': 
				fputs("; +\n  mov ax,[di]\n  inc ax\n  mov [di],ax\n", out);
				break;
			case '-': break;
			case '.': break;
			case ',': break;
			case '[': break;
				jmpId++;
			case ']': break;
			default: break;
		}
	}

	free(dest);
	free(out);

	return SUCCESS;
}

int main(int argc, const char *argv[])
{
	int status;
	if (argc != 2) {
		fprintf(stderr, "Usage: %s filename\n", argv[0]);
	}

	status = compile_bf(argv[1]);
	if (status != SUCCESS) {
		fprintf(stderr, "Compilation Error!\n");
	}

	return status;
}