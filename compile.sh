#!/bin/env bash

filename=${1%.*}


gcc -o bf-compile bf-compile.c
./bf-compile ${filename}.bf
nasm -s -f elf64 -o ${filename}.o ${filename}.asm
gcc -o ${filename} ${filename}.o