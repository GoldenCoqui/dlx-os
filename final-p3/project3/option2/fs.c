#include "fs.h"

int findFreeInode();
int findFreeBlock();
void markBlockAsFree(int blockIndex);
void markInodeAsFree(int inodeIndex);
struct DirectoryEntry* findDirectoryEntry(char* fname);

void mapfs(int fd) {
  if ((fs = (unsigned char*)mmap(NULL, FSSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
    perror("mmap failed");
    exit(EXIT_FAILURE);
  }
}


void unmapfs(){
  munmap(fs, FSSIZE);
}


void formatfs() {
  // Define and write the superblock structure
  struct Superblock superblock;
  superblock.numBlocks = FSSIZE / BLOCK_SIZE;
  superblock.numInodes = 100;  // Placeholder, adjust as needed

  // Calculate the size of the free list
  int sizeOfFreeList = superblock.numBlocks / 8;

  // Write the free list and inodes initialization logic
  unsigned char* freeList = (unsigned char*)malloc(sizeOfFreeList);
  memset(freeList, 0xFF, sizeOfFreeList);  // Set all bits to 1 (all blocks initially free)

  struct Inode* inodes = (struct Inode*)malloc(sizeof(struct Inode) * superblock.numInodes);

  // Write the superblock, free list, and inodes to the file system
  memcpy(fs, &superblock, sizeof(struct Superblock));
  memcpy(fs + sizeof(struct Superblock), freeList, sizeOfFreeList);
  memcpy(fs + sizeof(struct Superblock) + sizeOfFreeList, inodes, sizeof(struct Inode) * superblock.numInodes);

  free(freeList);
  free(inodes);
}



void loadfs() {
  // Read the superblock from the file system
  struct Superblock superblock;
  memcpy(&superblock, fs, sizeof(struct Superblock));

  // Calculate the size of the free list
  int sizeOfFreeList = superblock.numBlocks / 8;

  // Allocate memory for the free list and inodes
  unsigned char* freeList = (unsigned char*)malloc(sizeOfFreeList);
  struct Inode* inodes = (struct Inode*)malloc(sizeof(struct Inode) * superblock.numInodes);

  // Read the free list and inodes from the file system
  memcpy(freeList, fs + sizeof(struct Superblock), sizeOfFreeList);
  memcpy(inodes, fs + sizeof(struct Superblock) + sizeOfFreeList, sizeof(struct Inode) * superblock.numInodes);

  // Perform further processing as needed

  // Don't forget to free allocated memory when you're done using it
  free(freeList);
  free(inodes);
}



void lsfs() {
  // Read the superblock from the file system
  struct Superblock superblock;
  memcpy(&superblock, fs, sizeof(struct Superblock));

  // Calculate the size of the free list
  int sizeOfFreeList = superblock.numBlocks / 8;

  // Skip superblock and free list to get to the inodes
  int offset = sizeof(struct Superblock) + sizeOfFreeList;

  // Iterate through inodes to find directory entries
  for (int i = 0; i < superblock.numInodes; ++i) {
    struct Inode inode;
    memcpy(&inode, fs + offset + i * sizeof(struct Inode), sizeof(struct Inode));

    // Placeholder: Assume each inode corresponds to a directory
    // You may need additional logic based on your actual file system design
    printf("Directory: %s\n", inode.directBlocks[0]);  // Placeholder: Print the directory name

    // Iterate through directory entries
    for (int j = 0; j < inode.numBlocks; ++j) {
      struct DirectoryEntry entry;
      memcpy(&entry, fs + inode.directBlocks[j] * BLOCK_SIZE, sizeof(struct DirectoryEntry));

      // Placeholder: Print information about each file or directory
      printf("  %s\n", entry.name);
    }
  }
}

void addfilefs(char* fname) {
  // Implement logic to add a file to the file system
  // This involves creating directory entries, allocating inodes, and copying file data
  // Placeholder code assumes a simple file structure without directories

  // Create a new directory entry
  struct DirectoryEntry newEntry;
  strncpy(newEntry.name, fname, sizeof(newEntry.name));
  newEntry.inodeIndex = findFreeInode();  // Implement findFreeInode() based on your logic

  // Copy file data into the file system
  FILE* file = fopen(fname, "rb");
  fseek(file, 0, SEEK_END);
  size_t fileSize = ftell(file);
  fseek(file, 0, SEEK_SET);

  // Allocate data blocks for the file
  size_t numBlocks = (fileSize + BLOCK_SIZE - 1) / BLOCK_SIZE;
  newEntry.inode.numBlocks = numBlocks;
  newEntry.inode.fileSize = fileSize;

  for (size_t i = 0; i < numBlocks; ++i) {
    newEntry.inode.directBlocks[i] = findFreeBlock();  // Implement findFreeBlock() based on your logic
    fread(fs + newEntry.inode.directBlocks[i] * BLOCK_SIZE, 1, BLOCK_SIZE, file);
  }

  fclose(file);

  // Update the directory entry and inode in the file system
  // Placeholder: Write the code to update directory entry and inode in the file system
}

void removefilefs(char* fname) {
  // Implement logic to remove a file from the file system
  // This involves updating directory entries, releasing inodes, and freeing data blocks
  // Placeholder code assumes a simple file structure without directories

  // Find the directory entry for the file
  struct DirectoryEntry* entry = findDirectoryEntry(fname);  // Implement findDirectoryEntry() based on your logic

  if (entry != NULL) {
    // Free data blocks
    for (size_t i = 0; i < entry->inode.numBlocks; ++i) {
      // Implement logic to free data blocks
      markBlockAsFree(entry->inode.directBlocks[i]);  // Placeholder: Implement markBlockAsFree()
    }

    // Free the inode
    markInodeAsFree(entry->inodeIndex);  // Placeholder: Implement markInodeAsFree()

    // Update the directory entry in the file system
    // Placeholder: Write the code to update the directory entry in the file system
  }
}

void extractfilefs(char* fname) {
 // Implement logic to extract the contents of a file from the file system
  // This involves finding the file, reading its data blocks, and writing to a new file
  // Placeholder code assumes a simple file structure without directories

  // Find the directory entry for the file
  struct DirectoryEntry* entry = findDirectoryEntry(fname);  // Implement findDirectoryEntry() based on your logic

  if (entry != NULL) {
    // Create a new file for extraction
    FILE* extractedFile = fopen("extracted_file", "wb");  // Placeholder: Specify a proper output file name

    // Write data blocks to the new file
    for (size_t i = 0; i < entry->inode.numBlocks; ++i) {
      fwrite(fs + entry->inode.directBlocks[i] * BLOCK_SIZE, 1, BLOCK_SIZE, extractedFile);
    }

    fclose(extractedFile);
  }
}
