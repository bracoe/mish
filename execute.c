/*
 * execute.c
 *
 *  Created on: 5 Oct 2018
 *      Author: bram
 */


#include "execute.h"

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

/**
 *
 */
int dupPipe(int pip[2], int end, int destfd){
    if((end > 1) || (destfd > 2) ){
        fprintf(stderr, "Arguments for dupPipe are wrong!\n");
        return -1;
    }

    int ret = dup2(pip[end], destfd);
    if(ret < 0){
        perror("Duplication of pipe");
        fprintf(stderr, "dest file pipe: %d, des: %d, end: %d\n", pip[end], destfd, end);
        return -1;
    }

    return ret;
}

/**
 *
 */
int redirect(char *filename, int flags, int destfd){
    int fd = open(filename, flags);
    if(fd < 0){
        perror("Redirect");
        return -1;
    }

    int ret = dup2(fd, destfd);
    if(ret < 0){
        perror("Redirect dup");
    }

    return ret;
}
