//
// Created by Sarah Depew on 4/27/18.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <memory.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include "boolean.h"
#include "filesystem.h"

#define MINNUMARGUMENTS 2
#define MAXNUMARGUMENTS 3
#define FLAGLOCATION 1
#define FILELOCATION 2
#define FILELOCATIONBASIC 1
#define DEFAULTFILESIZE 1
#define AVERAGEFILESIZE 0.02
#define BLOCKSIZE 512

char *flag = "-s";
char *hash = "#";
char *deliminator = " ";

void help() {
    printf("Use format as follows: format <name of file to format>\n");
}

//Method that writes the disk image with the given size (filename is name of file and filesize is disk size to generate in mb)
void write_disk(char *file_name, float file_size) {
    long long int total_bytes = file_size * 1000000; //convert mb to bytes
    FILE *disk = fopen(file_name, "wb+"); //open the disk to write

    //compute the number of inodes
    //file_size of disksize right? Yes
    int num_inodes = ceilf((float) file_size / (float) AVERAGEFILESIZE);
    //why is it of long type? should be pretty small?
    long long int num_blocks_for_inodes = ceilf((float) (num_inodes * sizeof(inode)) / (float) BLOCKSIZE);

    //write boot block
    char boot[] = "bootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootbootboot";
    fwrite(boot, SIZEOFBOOTBLOCK, 1, disk);

    //write superblock
    superblock *superblock1 = malloc(sizeof(superblock));
    superblock1->size = BLOCKSIZE;
    superblock1->data_offset = num_blocks_for_inodes; //this is data region offset
    superblock1->inode_offset = 0;
    superblock1->free_block = 0;
    superblock1->free_inode = 0;
    superblock1->root_dir = 0; //default to first inode being the root directory
    fwrite(superblock1, SIZEOFSUPERBLOCK, 1, disk);
    free(superblock1);

    //write inode region
    inode *inodes[num_inodes];
    for (int i = 0; i < num_inodes; i++) {
        inodes[i] = malloc(sizeof(inode));
        inodes[i]->disk_identifier = 0;
        inodes[i]->parent_inode_index = -1;
        if (i == num_inodes - 1) {
            //make the last inode have a next of -1 to show end of free list
            inodes[i]->next_inode = -1;
        } else {
            inodes[i]->next_inode = i + 1;
        }
        inodes[i]->size = 0;
        inodes[i]->uid = 0;
        inodes[i]->gid = 0;
        inodes[i]->ctime = 0;
        inodes[i]->mtime = 0;
        inodes[i]->atime = 0;
        inodes[i]->type = 0;
        inodes[i]->permission = 0;
        inodes[i]->inode_index = i; //this is the only value that won't be replaced
        inodes[i]->i2block = -1;
        inodes[i]->i3block = -1;
        inodes[i]->last_block_index = -1;

        fwrite(inodes[i], sizeof(inode), 1, disk);
        free(inodes[i]);
    }
    //padding if needed. TODO. Added in the morning
    long padding_value =
            num_blocks_for_inodes * BLOCKSIZE - num_inodes * sizeof(inode); //total bytes minus those taken by inodes
    printf("number of inodes: %d\n", num_inodes);
    printf("number of bytes needed for inodes: %lld\n", num_blocks_for_inodes * BLOCKSIZE);
    printf("size of inode: %lu\n", sizeof(inode));
    printf("number of bytes for the inodes %lu\n", num_inodes * sizeof(inode));
    void *padding = malloc(sizeof(padding_value));
    fwrite(padding, padding_value, 1, disk);

    //write data region
    //TODO: make sure that the data blocks are linked into a list!
    long bytes_left_for_data_blocks =
            total_bytes - SIZEOFSUPERBLOCK - SIZEOFBOOTBLOCK - num_blocks_for_inodes * BLOCKSIZE;
    long num_data_blocks = ceilf((float) bytes_left_for_data_blocks / (float) BLOCKSIZE);
    printf("num data blocks %ld\n", num_data_blocks);

    for (int j = 0; j < num_data_blocks; j++) {
        //create the list before writing to the file!
        void *block_to_write = malloc(BLOCKSIZE); //malloc enough memory
        if (j == num_data_blocks - 1) {
            ((block *) block_to_write)->next_free_block = -1;
        } else {
            ((block *) block_to_write)->next_free_block = j + 1;
        }

        fwrite(block_to_write, BLOCKSIZE, 1, disk);
        free(block_to_write);
    }

    //done writing, so close the disk
    fclose(disk);
}

/*
 * Parse the command line input and ensure that the user has put in the correct information
 */
boolean parseCmd(char *integer, long long int *MB) {
    //(error checking gotten from stack overflow)
    const char *nptr = integer;                     /* string to read               */
    char *endptr = NULL;                            /* pointer to additional chars  */
    int base = 10;                                  /* numeric base (default 10)    */
    long long int number = 0;                       /* variable holding return      */

    /* reset errno to 0 before call */
    errno = 0;

    /* call to strtol assigning return to number */
    number = strtoll(nptr, &endptr, base);

    /* test return to number and errno values */
    if (nptr == endptr) {
        printf(" number : %lld  invalid  (no digits found, 0 returned)\n", number);
        return FALSE;
    } else if (errno == ERANGE && number == LONG_MIN) {
        printf(" number : %lld  invalid  (underflow occurred)\n", number);
        return FALSE;
    } else if (errno == ERANGE && number == LONG_MAX) {
        printf(" number : %lld  invalid  (overflow occurred)\n", number);
        return FALSE;
    } else if (errno == EINVAL) { /* not in all c99 implementations - gcc OK */
        printf(" number : %lld  invalid  (base contains unsupported value)\n", number);
        return FALSE;
    } else if (errno != 0 && number == 0) {
        printf(" number : %lld  invalid  (unspecified error occurred)\n", number);
        return FALSE;
    } else if (errno == 0 && nptr && *endptr != 0) {
        printf(" number : %lld    invalid  (since additional characters remain)\n", number);
        return FALSE;
    }

    //passed all the if statements and so can assigned the seconds value as the one input
    *MB = number;
    return TRUE;
}

//Please note that we are assuming a valid input is given here; specifically, we are assuming a valid number is put
int main(int argc, char *argv[]) {
    printf("number of arguments: %d\n", argc);
    printf("second argument: %s\n", argv[1]);
    printf("third argument: %s\n", argv[2]);

    if (argc != MINNUMARGUMENTS && argc != MAXNUMARGUMENTS) {
        help();
    } else {
        if (argc == MINNUMARGUMENTS) {
            printf("file name: %s\n", argv[FILELOCATIONBASIC]);
            write_disk(argv[FILELOCATIONBASIC], DEFAULTFILESIZE);
        } else {
            char *token = strtok(argv[FLAGLOCATION], deliminator);
            printf("token %s\n", token);

            printf("strcmp value %d\n", strcmp(flag, token));
            if (strcmp(flag, token) == 0) {
                token = strtok(NULL, deliminator);
                printf("token %s\n", token);

                token = strtok(token, hash);
                printf("number of megabytes %s\n", token);

                //convert the token to an integer value
                long long int MB;
                if (parseCmd(token, &MB) == TRUE) {
                    printf("number of megabytes after translation %lld\n", MB);
                    write_disk(argv[FILELOCATION], MB);
                } else {
                    help();
                }
            }
        }
    }
}