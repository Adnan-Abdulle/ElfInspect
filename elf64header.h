//
// Created by Adnan Abdulle on 2025-10-27.
//

#ifndef ELFINSPECT_ELF64HEADER_H
#define ELFINSPECT_ELF64HEADER_H
#include "elf_types.h"
int validateFields64(const Elf64_Ehdr* header32, char** errorMsg);
void printFields64(const Elf64_Ehdr* header32);
#endif //ELFINSPECT_ELF64HEADER_H
