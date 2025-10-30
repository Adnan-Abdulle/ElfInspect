//
// Created by Adnan Abdulle on 2025-10-27.
//

#ifndef ELFINSPECT_ELF32HEADER_H
#define ELFINSPECT_ELF32HEADER_H
#include "elf_types.h"

void validateFields32(const Elf32_Ehdr* header32);
void printFields32(const Elf32_Ehdr* header32);
#endif //ELFINSPECT_ELF32HEADER_H
