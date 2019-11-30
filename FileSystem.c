#include <stdio.h>
#include <stdlib.h>

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
Inode getAnInode(int INumber);
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
void initfs(char* filePath, int totalNumberOfBlocks, int totalNumberOfINodes)
void quit();

Inode getInode(int INumber) {
    Inode inode;
    lseek(fd, 2 * BLOCK_SIZE + INumber * INODE_SIZE, SEEK_SET);
    read(fd, %inode, INODE_SIZE);
    return inode;
}

void quit() {
    close(fd);
    exit(0);
}