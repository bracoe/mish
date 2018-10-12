/*
 * execute.c The implementation of the functions in the header file.
 * See the header file for more information.
 *
 *  Created on: 5 Oct 2018
 *      Author: Bram Coenen (tfy15bcn)
 */


#include "execute.h"

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

/* Duplicate a pipe to a standard I/O file descriptor
 * Arguments:	pip	the pipe
 *		end	tells which end of the pipe shold be dup'ed; it can be
 *			one of READ_END or WRITE_END
 *		destfd	the standard I/O file descriptor to be replaced
 * Returns:	-1 on error, else destfd
 */
int dupPipe(int pip[2], int end, int destfd){
    if((end > 1) || (destfd > 2) ){
        fprintf(stderr, "Arguments for dupPipe are wrong!\n");
        return -1;
    }

    int ret = dup2(pip[end], destfd);
    if(ret < 0){
        perror("Duplication of pipe");
        fprintf(stderr, "dest file pipe: %d, des: %d, end: %d\n", pip[end], \
        		destfd, end);
        return -1;
    }

    return ret;
}

/* Redirect a standard I/O file descriptor to a file
 * Arguments:	filename	the file to/from which the standard I/O file
 * 				descriptor should be redirected
 * 		flags	indicates whether the file should be opened for reading
 * 			or writing
 * 		destfd	the standard I/O file descriptor which shall be
 *			redirected
 * Returns:	-1 on error, else destfd
 */
int redirect(char *filename, int flags, int destfd){
    int fd = open(filename, flags);
    if(fd < 0){
        perror(filename);
        return -1;
    }

    int ret = dup2(fd, destfd);
    if(ret < 0){
        perror(filename);
    }

    return ret;
}
