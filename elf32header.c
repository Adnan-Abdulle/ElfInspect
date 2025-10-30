//
// Created by Adnan Abdulle on 2025-10-27.
//

#include "elf_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


void validateFields32(const Elf32_Ehdr* header32){
    if (!(header32->ident.EI_MAG0 == 0x7F &&
            header32->ident.EI_MAG1 == 0x45&&
            header32->ident.EI_MAG2 == 0x4C &&
            header32->ident.EI_MAG3 == 0x46 )){
        fprintf(stderr, "Error: Invalid magic numbers\n");
        exit(EXIT_FAILURE);
    }

    if (header32->e_ehsize != sizeof(Elf32_Ehdr)){
        fprintf(stderr, "Elf Header size does not match\n");
        exit(EXIT_FAILURE);
    }

    uint8_t class = header32->ident.EI_CLASS;
    if(class != 1) {
        fprintf(stderr, "Error: Expected Elf Class = 32bit\n");
        exit(EXIT_FAILURE);
    }

    uint8_t endi = header32->ident.EI_DATA;
    if(!(endi == 1 || endi == 2)){
        fprintf(stderr, "Error: Invalid data encoding\n");
        exit(EXIT_FAILURE);
    }

    uint32_t version = header32->e_version;
    if (version != 1){
        fprintf(stderr, "Error: Invalid version\n");
        exit(EXIT_FAILURE);
    }

    uint16_t type = header32->e_type;
    if(!(type >= 1 && type <= 4)){
        fprintf(stderr, "Error: No recognized file type\n");
        exit(EXIT_FAILURE);
    }

    uint16_t machine = header32->e_machine;
    if(!(machine == 62 || machine == 183 || machine == 243)){
        fprintf(stderr, "Error: No recognized machine type\n");
        exit(EXIT_FAILURE);
    }

    uint32_t entry = header32->e_entry;
    if (entry == 0){
        fprintf(stderr, "Warning: File has no associated entry point\n");
    }
}

void printFields32(const Elf32_Ehdr* header32){
    printf("Magic numbers: 0x%X %x %x %x\n",
           header32->ident.EI_MAG0,
           header32->ident.EI_MAG1,
           header32->ident.EI_MAG2,
           header32->ident.EI_MAG3
    );

    uint8_t class = header32->ident.EI_CLASS;
    if(class == 1){
        printf("Class: ELF32\n");
    } else if(class == 2) {
        printf("Class: ELF64\n");
    }
    uint8_t endi = header32->ident.EI_DATA;
    if(endi == 1){
        printf("Endianness: Little Endian\n");
    } else if(endi == 2) {
        printf("Endianness: Big Endian\n");
    }
    uint16_t type = header32->e_type;
    if(type == 1){
        printf("Type: Relocatable file (ET_REL)\n");
    } else if(type == 2){
        printf("Type: Executable (ET_EXEC)\n");
    } else if(type == 3){
        printf("Type: Shared object file (ET_DYN)\n");
    } else if(type == 4){
        printf("Type: Core file (ET_CORE)\n");
    }
    uint16_t machine = header32->e_machine;
    if(machine == 62){
        printf("Machine: x86-64 (EM_X86_64)\n");
    } else if(machine == 183){
        printf("Machine: ARM 64-bit (EM_AARCH64)\n");
    } else if(machine == 243){
        printf("Machine: RISC-V (EM_RISCV)\n");
    }

    printf("Entry point: 0x%x\n", header32->e_entry);

    printf("Version: %d\n", header32->e_version);
}

