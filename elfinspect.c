//
// Created by Adnan Abdulle on 2025-10-28.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <sys/file.h>
#include <sys/stat.h>
#include "write_read.h"
#include <stdint.h>
#define MAX_FILENAME_BYTES 2048

int connectSocket(char* path){

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        return -1;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof addr);

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);

    if (connect(fd, (struct sockaddr *)&addr, sizeof addr) == -1){
        perror("connect");
        close(fd);
        return -1;
    }
    return fd;
}

char* readFile(char* file, uint64_t * size){
    int fd = open(file, O_RDONLY);
    if (fd == -1) {
        perror("Error: open failed");
        return NULL;
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("Error: fstat failed");
        close(fd);
        return NULL;
    }
    if (!S_ISREG(sb.st_mode)){
        fprintf(stderr, "Error: This is not a regular file:%s\n", file);
        close(fd);
        return NULL;
    }

    *size = (uint64_t)sb.st_size;
    char* buffer = (char*) malloc(*size);
    ssize_t total = 0;

    while(total < (ssize_t)*size){
        ssize_t n = read(fd, buffer + total, (size_t)((ssize_t)*size - total));
        if (n < 0) {
            if (errno == EINTR) continue;
            perror("client: read failed");
            free(buffer);
            close(fd);
            return NULL;
        } else if (n == 0) {
            fprintf(stderr, "client: Error: Unexpected EOF.\n");
            free(buffer);
            close(fd);
            return NULL;
        }
        total += n;
    }
    close(fd);
    return buffer;
}

int writeToServer(int fd, const char* buffer, const char* filename, uint64_t size){
    size_t filename_len = strlen(filename);
    uint32_t raw_filename_len = (uint32_t)filename_len;

    if (filename_len == 0 || filename_len > MAX_FILENAME_BYTES) {
        fprintf(stderr, "Error: Filename length is invalid.\n");
        return -1;
    }

    if (write_fully(fd, &raw_filename_len, sizeof(raw_filename_len)) != sizeof(raw_filename_len)) {
        perror("Error: writeToFile failed\n");
        return -1;
    }


    if (write_fully(fd, &size, sizeof(size)) != sizeof(size)) {
        perror("Error:  writeToFile failed\n");
        return -1;
    }


    if (write_fully(fd, filename, filename_len) != (ssize_t)filename_len) {
        perror("Error:  writeToFile failed\n");
        return -1;
    }


    if (write_fully(fd, buffer, size) != (ssize_t)size) {
        perror("Error: writeToFile failed\n");
        return -1;
    }
    return 0;
}
int main(int argc, char* argv[]){
    if (argc != 3){
        fprintf(stderr, "Error: Invalid Arguements");
        return EXIT_FAILURE;
    }
    char* path = argv[1];
    char* elf_file = argv[2];
    char* fileBuff = NULL;
    uint64_t fileSize = 0;

    fileBuff = readFile((char *) elf_file, &fileSize);
    if (fileBuff == NULL){
        return EXIT_FAILURE;
    }
    int fd = connectSocket(path);
    if (fd == -1){
        fprintf(stderr, "Error: Failed to establish connection\n");
        goto cleanup;;
    }

    writeToServer(fd, fileBuff, elf_file, fileSize);

    char response_buffer[4096];
    ssize_t bytes_read;

    while ((bytes_read = read(fd, response_buffer, sizeof(response_buffer))) > 0) {
        write(STDOUT_FILENO, response_buffer, (size_t)bytes_read);
    }

    if (bytes_read < 0) {
        if (errno != EINTR) {
            perror("Error reading server response\n");
        }
    }
    cleanup:
    if (fd != -1) {
        shutdown(fd, SHUT_RDWR);
        close(fd);
    }
    free(fileBuff);

    return EXIT_SUCCESS;
}
