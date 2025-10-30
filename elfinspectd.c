//
// Created by Adnan Abdulle on 2025-10-27.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include "elf64header.h"
#include "elf_types.h"
#include "write_read.h"
#define MAX_FILENAME_BYTES 2048
#define MAX_FILE_SIZE (10 * 1024 * 1024)

int createSocket(char* path){
    if (unlink(path) == -1 && errno != ENOENT){
        perror("unlink");
        exit(EXIT_FAILURE);
    }

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof addr);

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);

    if (bind(fd, (struct sockaddr *)&addr, sizeof addr) == -1){
        perror("bind");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if (listen(fd, 4) == -1) {
        perror("listen");
        close(fd);
        exit(EXIT_FAILURE);
    }


    return fd;
}

int readFromClient(int client_fd, uint32_t* filename_len, uint64_t* file_size, char** buffer, char** filename){

    ssize_t bytes_read;

    bytes_read = read_fully(client_fd, filename_len, sizeof(*filename_len));
    if (bytes_read != (ssize_t)sizeof(*filename_len)) {
        fprintf(stderr, "Error: Failed to read filename length\n");
        return -1;
    }

    bytes_read = read_fully(client_fd, file_size, sizeof(*file_size));
    if (bytes_read != (ssize_t)sizeof(*file_size)) {
        fprintf(stderr, "Error: Failed to read file size.\n");
        return -1;
    }

    if (*filename_len == 0 || *filename_len > MAX_FILENAME_BYTES) {
        fprintf(stderr, "Error: Invalid filename length.\n");
        return -1;
    }
    if (*file_size == 0 || *file_size > MAX_FILE_SIZE) {
        fprintf(stderr, "Error: Invalid file size.\n");
        return -1;
    }

    *filename =  (char *) malloc(*filename_len + 1);
    if (!*filename) {
        perror("Error: malloc for filename failed.\n");
        return -1;
    }
    bytes_read = read_fully(client_fd, *filename, *filename_len);
    if (bytes_read != (ssize_t)*filename_len) {
        fprintf(stderr, "Error: Failed to read full filename.\n");
        free(*filename);
        return -1;
    }
    (*filename)[*filename_len] = '\0';

    *buffer =  malloc(*file_size);
    if (!*buffer){
        fprintf(stderr,"Error: malloc for buffer failed");
        free(*filename);
        return -1;
    }

    printf("Server: Reading file '%s'.\n", *filename);

    bytes_read = read_fully(client_fd, *buffer, *file_size);
    if (bytes_read != (ssize_t)*file_size) {
        fprintf(stderr, "Error: Failed to read full file data.\n");
        free(*buffer);
        free(*filename);
        return -1;
    }

    printf("File data received successfully. analyzing...\n");

    return 0;
}


void writeToClient(int client_fd, const Elf64_Ehdr *hdr) {
    char *buffer = NULL;
    size_t size = 0;

    FILE *mem = open_memstream(&buffer, &size);
    if (!mem) {
        perror("open_memstream failed");
        return;
    }

    char *errorMsg = NULL;

    if (!validateFields64(hdr, &errorMsg)) {
        fprintf(mem, "%s", errorMsg);
    } else {
        FILE *saved_stdout = stdout;
        stdout = mem;
        printFields64(hdr);
        stdout = saved_stdout;
    }

    fflush(mem);
    fclose(mem);

    write_fully(client_fd, buffer, size);

    free(buffer);

    shutdown(client_fd, SHUT_WR);
}


void handle_request(int client_fd) {
    uint32_t filename_len = 0;
    uint64_t file_size = 0;
    char *buffer = NULL;
    char *filename = NULL;

    if(readFromClient(client_fd, &filename_len, &file_size, &buffer, &filename) != 0){
        close(client_fd);
        return;
    }

    Elf64_Ehdr *hdr = (Elf64_Ehdr *)buffer;

    writeToClient(client_fd, hdr);

    free(filename);
    free(buffer);
    close(client_fd);

}

int main(int argc, char* argv[]){
    if (argc != 2){
        fprintf(stderr, "Error: Invalid Arguments\n");
        exit(EXIT_FAILURE);
    }

    int client_fd;
    int server_fd = createSocket(argv[1]);

    while(1){
        client_fd = accept(server_fd, NULL, NULL);
        if (client_fd == -1){
         if (errno == EINTR){
            continue;
        }
            perror("accept");
            continue;
        }
        printf("\nClient connected, handling request...\n");

        handle_request(client_fd);
    }
}
