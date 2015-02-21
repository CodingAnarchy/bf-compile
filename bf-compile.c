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
	int c, len, id;
	int jmpId = 0, jmpPtr = 0, bufSize = 0, getId = 0;
	char jmpStack[100];

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
	fputs("BITS 64\n\n", out);
	fputs("; ", out);
	fputs(dest, out);
	fputs("\n; compiled from ", out);
	fputs(filename, out);
	fputs("\n; assemble: nasm -f elf64 -o ", out);
	fputs(obj, out);
	fputs(" ", out);
	fputs(dest, out);
	fputs("\n; link: gcc -o <cmd> ", out);
	fputs(obj, out);
	fputs("\n;\n;\nglobal main\nextern putchar, getchar\n\nsection .bss\nbuf resb 8192\n\n", out); 
	fputs("section .text\n", out);
	fputs("main:\n  xor rbx,rbx\n  mov rbx,buf+1\n", out);

	// Start of program in assembly file

	// Read in characters and write out assembly to handle them
	while ((c = fgetc(source)) != EOF) {
		switch(c) {
			case '>':
				fputs("; >\n  add rbx,8\n", out);
				break;
			case '<': 
				fputs("; <\n  sub rbx,8\n", out);
				break;
			case '+': 
				fputs("; +\n  mov rax,[rbx]\n  inc rax\n  mov [rbx],rax\n", out);
				break;
			case '-':
				fputs("; -\n  mov rax,[rbx]\n  dec rax\n  mov [rbx],rax\n", out);
				break;
			case '.': 
				fputs("; .\n  mov rax,[rbx]\n  mov rdi,rax\n  call putchar\n", out);
				break;
			case ',': 
				fputs("; ,\n.getch", out);
				fprintf(out, "%d", getId);
				fputs(":\n  call getchar\n  mov rdi,rax\n  cmp rax,0\n  jl .getch", out);
				fprintf(out, "%d", getId);
				fputs("\n  mov [rbx],rax\n", out);
				getId++;
				break;
			case '[':
				fputs("; [\n.loops", out);
				fprintf(out, "%d", jmpId);
				fputs(":\n  mov rax,[rbx]\n  cmp rax,0\n  je .loope", out);
				fprintf(out, "%d", jmpId);
				fputs("\n", out);
				jmpStack[jmpPtr] = jmpId;
				jmpId++;
				jmpPtr++;
				break;
			case ']':
				jmpPtr--;
				id = jmpStack[jmpPtr];
				fputs("; ]\n  mov rax,[rbx]\n  cmp rax,0\n  jne .loops", out);
				fprintf(out, "%d", id);
				fputs("\n.loope", out);
				fprintf(out, "%d", id);
				fputs(":\n", out);
				break;
			default: break;
		}
	}

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