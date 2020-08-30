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
#include <linux/types.h>

#define         MAX_LINE_LEN            1024
#define         MAX_LINES               256

#define         PROCESS_START_ADDRESS   1
#define         PROCESS_END_ADDRESS     2

void printUsage() {
        printf("error\n");
}

struct encryptable_range {
        int num;
        unsigned long startAddress;
        unsigned long endAddress;
        struct encryptable_range* next;
};

struct encryptable_range* ranges = 0;

void printRangesList() {
        printf("\n\n\n\n");

        struct encryptable_range* cur_range = ranges;

        while (cur_range != 0) {
                printf("num: %d    start:  %lx   end:  %lx\n", cur_range->num, 
                        cur_range->startAddress, cur_range->endAddress);
                cur_range = cur_range->next;
        }
}

void printSymTab(Elf64_Shdr* shdr, Elf64_Shdr* strHdr, uint8_t *mem) {
        size_t dynsym_off = shdr->sh_offset;
        size_t dynstr_off = strHdr->sh_offset;
        size_t dynsym_sz = shdr->sh_size;

        printf("STRTAB offset %lx\n", dynstr_off);

        for (size_t j = 0; j * sizeof(Elf64_Sym) < dynsym_sz; j++) {
                Elf64_Sym sym;
                size_t absoffset = dynsym_off + j * sizeof(Elf64_Sym);
                memmove(&sym, mem + absoffset, sizeof(sym));
                printf("SYMBOL TABLE ENTRY %zd\n", j);
                printf("st_name = %d", sym.st_name);
                
                if (sym.st_name != 0) {
                        printf(" (%s)", (char*)(mem + dynstr_off + sym.st_name));
                }

                printf("\n");
                printf("st_info = %d\n", sym.st_info);
                printf("st_other = %d\n", sym.st_other);
                printf("st_shndx = %d\n", sym.st_shndx);
                printf("st_value = %p\n", (void *)sym.st_value);
                printf("st_size = %zd\n", sym.st_size);
                printf("\n");
        }
}

int parseSymbolNumber(char* name, int addrType) {
        int num;

        if (addrType == PROCESS_START_ADDRESS) {
                if (strstr(name, ".START_DECRYPT\0")) {
                        num = atoi(name + 14);
                }
        }

        if (addrType == PROCESS_END_ADDRESS) {
                if (strstr(name, ".END_DECRYPT\0")) {
                        num = atoi(name + 12);
                }
        }
        
        return num;
}

void setAddressBasedOnType(struct encryptable_range* range, Elf64_Addr addr, int addrType) {
        if (addrType == PROCESS_START_ADDRESS) {
                range->startAddress = addr;
        }

        if (addrType == PROCESS_END_ADDRESS) {
                range->endAddress = addr;
        }
}

void addAddressToEncryptListItem(int num, Elf64_Addr addr, int addrType) {
        int found = 0;
        struct encryptable_range* cur_range = ranges;

        if (ranges == 0) {
                ranges = malloc(sizeof(struct encryptable_range));
                ranges->num = num;
                setAddressBasedOnType(ranges, addr, addrType);
                ranges->next = 0;
                return;
        }

        while (cur_range != 0) {
                if (cur_range->num == num) {
                        found = 1;
                        setAddressBasedOnType(cur_range, addr, addrType);
                        break;
                }

                cur_range = cur_range->next;
        }

        if (!found) {
                struct encryptable_range* next_range = malloc(sizeof(struct encryptable_range));
                next_range->num = num;
                setAddressBasedOnType(next_range, addr, addrType);
                next_range->next = 0;
                next_range->next = ranges;
                ranges = next_range;
        }
}

void addSymbolToEncryptionList(char* symName, Elf64_Addr symAddr) {
        if (strstr(symName, "END_DECRYPT\0")) {
                int symNum = parseSymbolNumber(symName, PROCESS_END_ADDRESS);
                printf("sym num: %s    start:   %d\n", symName, symNum);
                addAddressToEncryptListItem(symNum, symAddr, PROCESS_END_ADDRESS);
        }

        if (strstr(symName, "START_DECRYPT\0")) {
                int symNum = parseSymbolNumber(symName, PROCESS_START_ADDRESS);
                printf("sym num: %s    end:   %d\n", symName, symNum);
                addAddressToEncryptListItem(symNum, symAddr, PROCESS_START_ADDRESS);
        }
}

void processSymTab(Elf64_Shdr* shdr, Elf64_Shdr* strHdr, uint8_t *mem) {
        size_t dynsym_off = shdr->sh_offset;
        size_t dynstr_off = strHdr->sh_offset;
        size_t dynsym_sz = shdr->sh_size;

        for (size_t j = 0; j * sizeof(Elf64_Sym) < dynsym_sz; j++) {
                Elf64_Sym sym;
                size_t absoffset = dynsym_off + j * sizeof(Elf64_Sym);
                memmove(&sym, mem + absoffset, sizeof(sym));
                
                if (sym.st_name != 0 && strstr((char*)(mem + dynstr_off + sym.st_name), 
                        "END_DECRYPT\0")) {
                        printf(" (%s)", (char*)(mem + dynstr_off + sym.st_name));
                        printf("\n");
                        printf("st_info = %d\n", sym.st_info);
                        printf("st_other = %d\n", sym.st_other);
                        printf("st_shndx = %d\n", sym.st_shndx);
                        printf("st_value = %p\n", (void *)sym.st_value);
                        printf("st_size = %zd\n", sym.st_size);
                        printf("\n");

                        addSymbolToEncryptionList((char*)(mem + dynstr_off + sym.st_name),
                                sym.st_value);
       
                }

                if (sym.st_name != 0 && strstr((char*)(mem + dynstr_off + sym.st_name), 
                        "START_DECRYPT\0")) {
                        printf(" (%s)", (char*)(mem + dynstr_off + sym.st_name));
                        printf("\n");
                        printf("st_info = %d\n", sym.st_info);
                        printf("st_other = %d\n", sym.st_other);
                        printf("st_shndx = %d\n", sym.st_shndx);
                        printf("st_value = %p\n", (void *)sym.st_value);
                        printf("st_size = %zd\n", sym.st_size);
                        printf("\n");

                        addSymbolToEncryptionList((char*)(mem + dynstr_off + sym.st_name),
                                sym.st_value);
       
                }
        }
}

Elf64_Shdr* getSectionStrTabHeader(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr) {
        Elf64_Shdr* cur_section_header;
        cur_section_header = &shdr[ehdr->e_shstrndx];

        if (cur_section_header->sh_type == SHT_STRTAB) {
                return cur_section_header;
        } else {
                printf("Unexpected error\n");
                exit(-1);
        }
}

Elf64_Shdr* getSymTabHeader(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr) {
        for (int i = 0; i < ehdr->e_shnum; ++i) {
                Elf64_Shdr* cur_section_header;
                cur_section_header = &shdr[i];

                if (cur_section_header->sh_type == SHT_SYMTAB) {
                       return cur_section_header; 
                }
        }

        return 0;
}

Elf64_Shdr* getStrTabHeader(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr, Elf64_Shdr* strShdr, uint8_t* mem) {
        char* strTabName = ".strtab\0";

        for (int i = 0; i < ehdr->e_shnum; ++i) {
                Elf64_Shdr* cur_section_header;
                cur_section_header = &shdr[i];

                if (cur_section_header->sh_type == SHT_STRTAB &&
                        strcmp(strTabName, 
                                mem + 
                                strShdr->sh_offset +
                                cur_section_header->sh_name) == 0) {
                       return cur_section_header; 
                }
        }

        return 0;
}

uint8_t* mapFileIntoMemory(char* fileName) {
        int fd;

        if ((fd = open(fileName, O_RDWR)) < 0) {
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

        if (fd != 0) {
                close(fd);
        }

        return mem;
}

int main(int argc, char** argv) {
	if (argc != 2) {
                printUsage();
                exit(0);
        }

	uint8_t *mem = mapFileIntoMemory(argv[1]);

        Elf64_Ehdr *ehdr;
        Elf64_Phdr *phdr;
        Elf64_Shdr *shdr;

        ehdr = (Elf64_Ehdr *)mem;
        phdr = (Elf64_Phdr *)&mem[ehdr->e_phoff];
        shdr = (Elf64_Shdr *)&mem[ehdr->e_shoff];

        Elf64_Shdr* sh_strtab_section_header = getSectionStrTabHeader(ehdr, shdr);
        printf("shstrb\n");
        Elf64_Shdr* cur_symtab_section_header = getSymTabHeader(ehdr, shdr);
        printf("symtab\n");
        Elf64_Shdr* cur_strtab_section_header = getStrTabHeader(ehdr, shdr, sh_strtab_section_header, mem);
        printf("strtab\n");

        if (cur_strtab_section_header == 0 || cur_symtab_section_header == 0) {
                printf("Missing sections\n");
                exit(-1);
        }

        processSymTab(cur_symtab_section_header, cur_strtab_section_header, mem);
        printRangesList();
}
