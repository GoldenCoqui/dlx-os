// fs.h

#ifndef __FS_H__
#define __FS_H__

#include <sys/mman.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FSSIZE 10000000
#define BLOCK_SIZE 512

extern unsigned char* fs;

// Superblock structure
struct Superblock {
  int numBlocks;    // Total number of blocks in the file system
  int numInodes;    // Number of inodes in the file system
  // Add any other superblock information you need
};

// Inode structure
struct Inode {
  int numBlocks;    // Number of data blocks used by the file
  int fileSize;     // Size of the file in bytes
  int directBlocks[100];  // Placeholder for direct block references
  // Add any other inode information you need
};

// Directory entry structure
struct DirectoryEntry {
  char name[255];   // Name of the file or directory
  int inodeIndex;   // Index of the associated inode
  struct Inode inode;  // Placeholder for the inode structure associated with the entry
};

void mapfs(int fd);
void unmapfs();
void formatfs();
void loadfs();
void lsfs();
void addfilefs(char* fname);
void removefilefs(char* fname);
void extractfilefs(char* fname);

#endif
