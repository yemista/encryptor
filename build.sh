#!/bin/bash

gcc -S -fPIC -fno-asynchronous-unwind-tables encryptor.c -o encryptor.s

script to modify assmebly called here

as -g -L  encryptor.s -o encryptor.o

script to encrypt functions called here

ld /usr/lib/x86_64-linux-gnu/crti.o /usr/lib/x86_64-linux-gnu/crtn.o /usr/lib/x86_64-linux-gnu/crt1.o -lc encryptor.o -dynamic-linker
	/lib64/ld-linux-x86-64.so.2 -o encryptor

run elfeditor against binary
