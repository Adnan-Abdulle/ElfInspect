//#include <stdio.h>
//#include <stdlib.h>
//#include <sys/stat.h>
//#include <unistd.h>
//#include <fcntl.h>
//#include <errno.h>
//#include <stdint.h>
//#include "elf_types.h"
//#include "elf64header.h"
//#include "elf32header.h"
//
//
//char *validateArguments(int argc, char *argv[]){
//    if (argc != 2){
//        fprintf(stderr, "Error: Invalid Arguments\n");
//        return NULL;
//    }
//    return argv[1];
//}
//
//void validateFile(char *file){
//
//    int fd;
//    struct stat sb;
//    fd = open(file, O_RDONLY);
//    if(fd == -1)
//    {
//        perror("open");
//        exit(EXIT_FAILURE);
//    }
//    if(fstat(fd, &sb) == -1)
//    {
//        perror("fstat");
//        close(fd);
//        exit(EXIT_FAILURE);
//    }
//    if (!S_ISREG(sb.st_mode)){
//        fprintf(stderr, "Error: This is not a regular file:%s\n", file);
//        close(fd);
//        exit(EXIT_FAILURE);
//    }
//    if ( (size_t)sb.st_size < sizeof(Elf32_Ehdr)) {
//        fprintf(stderr, "Error: File: %s is to small to be an ELF file\n", file);
//        exit(EXIT_FAILURE);
//    }
//
//    close(fd);
//
//    printf("Valid file: Yes\n");
//}
//
//ssize_t readHeader(char *file, e_ident* ident){
//    int fd;
//    size_t header = 0;
//    fd = open(file, O_RDONLY);
//    if(fd == -1)
//    {
//        perror("open");
//        exit(EXIT_FAILURE);
//    }
//    ssize_t count = sizeof(e_ident);
//    ssize_t total = 0;
//    unsigned char *buf = (unsigned char*) ident;
//    while (total < count) {
//        ssize_t n = read(fd, buf + total, (size_t)(count - total));
//        if (n > 0) {
//            total += n;
//        } else if (n == 0) {
//            fprintf(stderr, "Error: Unexpected eof in elf header\n");
//            close(fd);
//        } else if (errno == EINTR) {
//            continue;
//        } else {
//            perror("read");
//            close(fd);
//            return -1;
//        }
//    }
//
//    if (total != sizeof(e_ident)) {
//        fprintf(stderr, "Error: Size of Ident array does not match\n");
//        exit(EXIT_FAILURE);
//    }
//
//    if (ident->EI_CLASS == 1){
//        header = sizeof(Elf32_Ehdr);
//    } else if (ident->EI_CLASS == 2){
//        header = sizeof(Elf64_Ehdr);
//    } else {
//        fprintf(stderr, "Error: Invalid ELF class\n");
//        close(fd);
//        exit(EXIT_FAILURE);
//    }
//
//    size_t headerBits = header - count;
//    size_t remainingBits = 0;
//    while (remainingBits < headerBits) {
//        ssize_t n = read(fd, buf + count + remainingBits, headerBits - remainingBits);
//        if (n > 0) {
//            remainingBits += n;
//        } else if (n == 0) {
//            fprintf(stderr, "Error: Unexpected EOF while reading remaining header\n");
//            close(fd);
//            exit(EXIT_FAILURE);
//        } else if (errno == EINTR) {
//            continue;
//        } else {
//            perror("read");
//            close(fd);
//            return -1;
//        }
//    }
//
//    if (count + remainingBits != header){
//        fprintf(stderr, "Error: Unable to read full ELF header\n");
//        exit(EXIT_FAILURE);
//    }
//    close(fd);
//
//    return (ssize_t)header;
//}
//
//
//int main(int argc, char *argv[]) {
//
//    char *file = validateArguments(argc, argv);
//    ElfHeader_Union header;
//
//    if (file == NULL){
//        return EXIT_FAILURE;
//    }
//
//    printf("file: %s\n", file);
//    validateFile(file);
//    ssize_t n = readHeader(file, &header.ident);
//    if (n == -1){
//        return EXIT_FAILURE;
//    }
//    if (header.ident.EI_CLASS == 1){
//        validateFields32(&header.header32);
//        printFields32(&header.header32);
//    } else if(header.ident.EI_CLASS == 2){
//        validateFields64(&header.header64);
//        printFields64(&header.header64);
//    }
//
//    return EXIT_SUCCESS;
//}
