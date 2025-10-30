cmake_minimum_required(VERSION 3.10)
project(elfinspect C)

set(CMAKE_C_STANDARD 11)

add_executable(elfinspect main.c
        elf32header.c
        elf32header.h
        elf64header.c
        elf64header.h
        elf_types.h
        elfinspectd.c
        elfinspect.c
        write_read.c
        write_read.h
)
