#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>

void printUsage() {

}

void printProgramHeader(Elf64_Phdr* phdr) {
	if (phdr->p_type == PT_LOAD) {
		printf("Program header at %x has flags %x\n", (unsigned int)phdr, phdr->p_flags);
	}
}

void markSectionHeaderReadWrite(Elf64_Shdr* shdr) {
        shdr->sh_flags = 0x7;//SHF_WRITE | SHF_ALLOC;
}

void markProgramHeaderReadWrite(Elf64_Phdr* phdr) {
	if (phdr->p_type == PT_LOAD) {
		phdr->p_flags = 0x7;//PF_W | PF_R;
	}
}

void saveModifiedElfFile(char* origFileName, uint8_t* buff, struct stat* st) {
	int fdo = open("memdump_log", O_WRONLY | O_CREAT, 0644);
	write(fdo, buff, st->st_size);
	close(fdo);
}

int main(int argc, char** argv) {
	if (argc != 2) {
		printUsage();
		exit(0);
	}

	int fd;

	if ((fd = open(argv[1], O_RDWR)) < 0) {
		perror("open");
		exit(-1);
	}

	struct stat st;

	if (fstat(fd, &st) < 0) {
		perror("fstat");
		exit(-1);
	}

	uint8_t *mem = mmap(NULL, st.st_size, PROT_WRITE, MAP_PRIVATE, fd, 0);

	if (mem == MAP_FAILED) {
		perror("mmap");
		exit(-1);
	}

	Elf64_Ehdr *ehdr;
	Elf64_Phdr *phdr;
	Elf64_Shdr *shdr;

	ehdr = (Elf64_Ehdr *)mem;
	phdr = (Elf64_Phdr *)&mem[ehdr->e_phoff];
	shdr = (Elf64_Shdr *)&mem[ehdr->e_shoff];

	printf("Number of program headers:  %d\n", ehdr->e_phnum);

	for (int i = 0; i < ehdr->e_phnum; ++i) {
		Elf64_Phdr* cur_program_header = &phdr[i];
		printProgramHeader(cur_program_header);

			markProgramHeaderReadWrite(cur_program_header);
	}

	printf("Number of section headers:  %d\n", ehdr->e_shnum);

	for (int i = 0; i < ehdr->e_shnum; ++i) {
		Elf64_Shdr* cur_section_header = &shdr[i];

		if (i == 15) {
			markSectionHeaderReadWrite(cur_section_header);
		}
	}

	saveModifiedElfFile(argv[1], mem, &st);
	close(fd);
}
