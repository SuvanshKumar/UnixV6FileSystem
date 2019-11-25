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

typedef struct
{
        unsigned short inode;
        char filename[14];
} directoryEntry;

superBlockType super;
int fd;
char pwd[100];
int curINodeNumber;
char fileSystemPath[100];
