/*
BF-Compile.c
C program to compile brainfuck to assembly
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUCCESS 0

int compile_bf(const char *filename) {

	unsigned int ptr = 0;
	FILE *source;
	FILE *out;
	char *dest, *obj, *dot;
	int c, len;
	int jmpId = 0, bufSize = 0;
	int *jmpStack;

	if ((source = fopen(filename, "rb")) == NULL) {
		fprintf(stderr, "Could not open file %s", filename);
		return -1;
	}
	
	dot = strrchr(filename, '.');
	if (dot) len = dot - filename;
	else len = strlen(filename);
	dest = malloc(len + 5);
	strncpy(dest, filename, len);
	strcpy(dest + len, ".asm");
	obj = malloc(len + 5);
	strncpy(obj, filename, len);
	strcpy(obj + len, ".o");

	if ((out = fopen(dest, "wb")) == NULL) {
		fprintf(stderr, "Could not create file %s", dest);
		return -1;
	}	

	// Header information in assembly file
	fputs("; ", out);
	fputs(dest, out);
	fputs("\n; compiled from ", out);
	fputs(filename, out);
	fputs("\n; assemble: nasm -f elf64 -o ", out);
	fputs(obj, out);
	fputs(" ", out);
	fputs(dest, out);
	fputs("\n; link: ld ", out);
	fputs(obj, out);
	fputs(" <cmd>\n", out);
	fputs(";\n;\nglobal _start\nextern putchar\n\nsection .text\n", out);
	fputs("_start:\n", out);

	// Start of program in assembly file
	fputs("  mov ebx,buffer\n", out);
	fputs("  mov edi,ebx\n", out);

	// Read in characters and write out assembly to handle them
	while ((c = fgetc(source)) != EOF) {
		switch(c) {
			case '>':
				fputs("; >\n  inc edi\n", out);
				bufSize++;
				break;
			case '<': 
				fputs("; <\n  dec edi\n", out);
				break;
			case '+': 
				fputs("; +\n  mov rax,[edi]\n  inc rax\n  mov [edi],rax\n", out);
				break;
			case '-':
				fputs("; +\n  mov rax,[edi]\n  dec rax\n  mov [edi],rax\n", out);
				break;
			case '.': 
				fputs("; .\n  mov rax,[edi]\n  push rax\n  call putchar\n", out);
				break;
			case ',': break;
			case '[':
				jmpId++;
				break;
			case ']': break;
			default: break;
		}
	}
	fputs("\n\nsection .data\n\nbuffer: times ", out);
	fprintf(out, "%d", bufSize);
	fputs(" db 0\n", out);

	free(dest);
	free(obj);

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