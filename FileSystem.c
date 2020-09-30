#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define BLOCK_SIZE 1024
#define MAX_FILE_SIZE 4194304 // 4GB of file size
#define FREE_ARRAY_SIZE 248 // free and inode array size
#define INODE_SIZE 64


/*************** super block structure**********************/
typedef struct {
    unsigned int isize; // 4 byte
    unsigned int fsize;
    unsigned int nfree;
    unsigned short free[FREE_ARRAY_SIZE];
    unsigned int ninode;
    unsigned short inode[FREE_ARRAY_SIZE];
    unsigned short flock;
    unsigned short ilock;
    unsigned short fmod;
    unsigned int time[2];
} superBlockType;

/****************inode structure ************************/
typedef struct {
    unsigned short flags;       // 2 bytes
    char nlinks;                // 1 byte
    char uid;
    char gid;
    unsigned int size;          // 32bits  2^32 = 4GB filesize
    unsigned short addr[22];    // to make total size = 64 byte inode size
    unsigned int actime;
    unsigned int modtime;
} Inode;

typedef struct {
    unsigned short inode;
    char filename[14];
} directoryEntry;

superBlockType superBlock;
int fileDescriptor, currentINodeNumber, totalINodesCount;
char currentWorkingDirectory[100];
char fileSystemPath[100];

// function declarations for the main method to use
void writeBufferToBlock(int blockNumber, void* buffer, int numberOfBytes);
void writeToBlockWithOffset(int blockNumber, int offset, void* buffer, int numberOfBytes);
void readFromBlockWithOffset(int blockNumber, int offset, void* buffer, int numberOfBytes);
void addAFreeBlock(int blockNumber);
void getAFreeBlock();
void addAFreeInode(int iNumber);
Inode getInode(int INumber);
int getAFreeInode();
void writeTheInode(int INumber, Inode inode);
void initializeRootDirectory();
void ls();
void makeDirectory(char* directoryName);
int findLastIndex(char str[100], char ch);  // String size (and currentWorkingDirectory size is hard-coded to 100 for now)
void sliceString(const char * str, char * buffer, size_t startPosition, size_t endPosition);
void changeDirectory(char* directoryName);
void copyIn(char* sourceFilePath, char* fileName);
void copyOut(char* destinationFilePath, char* fileName);
void removeFile(char* fileName);    // Implements the rm command
void removeDirectory(char* directoryName);
void openFileSystem(const char *fileName);
void initfs(char* filePath, int totalNumberOfBlocks, int totalNumberOfINodes);
void quit();

Inode getInode(int INumber) {
    Inode inode;
    lseek(fileDescriptor, 2 * BLOCK_SIZE + INumber * INODE_SIZE, SEEK_SET);
    read(fileDescriptor, &inode, INODE_SIZE);
    return inode;
}

void writeBufferToBlock(int blockNumber, void* buffer, int numberOfBytes)
{
    lseek(fileDescriptor, BLOCK_SIZE * blockNumber, SEEK_SET);
    write(fileDescriptor, buffer, numberOfBytes);
}

void writeToBlockWithOffset(int blockNumber, int offset, void* buffer, int numberOfBytes)
{
    lseek(fileDescriptor, (BLOCK_SIZE * blockNumber) + offset, SEEK_SET);
    write(fileDescriptor, buffer, numberOfBytes);
}

void readFromBlockWithOffset(int blockNumber, int offset, void* buffer, int numberOfBytes)
{
    lseek(fileDescriptor, (BLOCK_SIZE * blockNumber) + offset, SEEK_SET);
    read(fileDescriptor, buffer, numberOfBytes);
}

void quit() {
    close(fileDescriptor);
    exit(0);
}

int main(int argc, char *argv[])
{
    char c;

    printf("\n Clearing screen \n");
    system("clear");

    unsigned int block_number =0, inode_number=0;
    char *fs_path;
    char *arg1, *arg2;
    char *my_argv, cmd[256];

    while(1)
    {
        printf("\n%s@%s>>>",fileSystemPath,currentWorkingDirectory);
        scanf(" %[^\n]s", cmd);
        my_argv = strtok(cmd," ");

        if(strcmp(my_argv, "initfs")==0)
        {

            fs_path = strtok(NULL, " ");
            arg1 = strtok(NULL, " ");
            arg2 = strtok(NULL, " ");
            if(access(fs_path, X_OK) != -1)
            {
                printf("filesystem already exists. \n");
                printf("same file system will be used\n");
            }
            else
            {
                if (!arg1 || !arg2)
                    printf(" insufficient arguments to proceed\n");
                else
                {
                    block_number = atoi(arg1);
                    inode_number = atoi(arg2);
                    initfs(fs_path,block_number, inode_number);
                }
            }
            my_argv = NULL;
        }
        else if(strcmp(my_argv, "q")==0){
            quit();
        }
        else if(strcmp(my_argv, "ls")==0){
            ls();
        }
        else if(strcmp(my_argv, "mkdir")==0){
            arg1 = strtok(NULL, " ");
            makeDirectory(arg1);
        }
        else if(strcmp(my_argv, "cd")==0){
            arg1 = strtok(NULL, " ");
            changedir(arg1);
        }
        else if(strcmp(my_argv, "cpin")==0){
            arg1 = strtok(NULL, " ");
            arg2 = strtok(NULL, " ");
            copyIn(arg1,arg2);
        }
        else if(strcmp(my_argv, "cpout")==0){
            arg1 = strtok(NULL, " ");
            arg2 = strtok(NULL, " ");
            copyOut(arg1,arg2);
        }
        else if(strcmp(my_argv, "rm")==0){
            arg1 = strtok(NULL, " ");
            rm(arg1);
        }
        else if(strcmp(my_argv, "currentWorkingDirectory")==0){
            printf("%s\n",currentWorkingDirectory);
        }

    }
}
