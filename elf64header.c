//
// Created by Adnan Abdulle on 2025-10-27.
//

#include "elf64header.h"

#include "elf_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

 int validateFields64(const Elf64_Ehdr* header64, char** errorMsg){
    if (!(header64->ident.EI_MAG0 == 0x7F &&
          header64->ident.EI_MAG1 == 0x45&&
          header64->ident.EI_MAG2 == 0x4C &&
          header64->ident.EI_MAG3 == 0x46 )){
        *errorMsg = "Error: Invalid magic numbers\n";
        return 0;
    }

    if (header64->e_ehsize != sizeof(Elf64_Ehdr)){
        *errorMsg ="Elf Header size does not match\n";
        return 0;
    }

    uint8_t class = header64->ident.EI_CLASS;
    if(class != 2) {
        *errorMsg = "Error: Expected Elf Class = 64bit\n";
        return 0;
    }

    uint8_t endi = header64->ident.EI_DATA;
    if(!(endi == 1 || endi == 2)){
        *errorMsg ="Error: Invalid data encoding\n";
        return 0;
    }

    uint32_t version = header64->e_version;
    if (version != 1){
        *errorMsg = "Error: Invalid version\n";
        return 0;
    }

    uint16_t type = header64->e_type;
    if(!(type >= 1 && type <= 4)){
        *errorMsg = "Error: No recognized file type\n";
        return 0;
    }

    uint16_t machine = header64->e_machine;
    if(!(machine == 62 || machine == 183 || machine == 243)){
        *errorMsg = "Error: No recognized machine type\n";
        return 0;
    }

    uint64_t entry = header64->e_entry;
    if (entry == 0){
        *errorMsg = "Warning: File has no associated entry point\n";
    }
     return 1;
}

void printFields64(const Elf64_Ehdr* header64){
    printf("Magic numbers: 0x%X %x %x %x\n",
           header64->ident.EI_MAG0,
           header64->ident.EI_MAG1,
           header64->ident.EI_MAG2,
           header64->ident.EI_MAG3
    );

    uint8_t class = header64->ident.EI_CLASS;
    if(class == 1){
        printf("Class: ELF32\n");
    } else if(class == 2) {
        printf("Class: ELF64\n");
    }
    uint8_t endi = header64->ident.EI_DATA;
    if(endi == 1){
        printf("Endianness: Little Endian\n");
    } else if(endi == 2) {
        printf("Endianness: Big Endian\n");
    }
    uint16_t type = header64->e_type;
    if(type == 1){
        printf("Type: Relocatable file (ET_REL)\n");
    } else if(type == 2){
        printf("Type: Executable (ET_EXEC)\n");
    } else if(type == 3){
        printf("Type: Shared object file (ET_DYN)\n");
    } else if(type == 4){
        printf("Type: Core file (ET_CORE)\n");
    }
    uint16_t machine = header64->e_machine;
    if(machine == 62){
        printf("Machine: x86-64 (EM_X86_64)\n");
    } else if(machine == 183){
        printf("Machine: ARM 64-bit (EM_AARCH64)\n");
    } else if(machine == 243){
        printf("Machine: RISC-V (EM_RISCV)\n");
    }

    printf("Entry point: 0x%" PRIx64 "\n", header64->e_entry);

    printf("Version: %d\n", header64->e_version);
}

