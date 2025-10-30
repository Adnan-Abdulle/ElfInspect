//
// Created by Adnan Abdulle on 2025-10-28.
//

#ifndef ELFINSPECT_WRITE_READ_H
#define ELFINSPECT_WRITE_READ_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <sys/file.h>
#include <sys/stat.h>
ssize_t write_fully(int fd, const void *buf, size_t n);
ssize_t read_fully(int fd, void *buf, size_t n);
#endif //ELFINSPECT_WRITE_READ_H
