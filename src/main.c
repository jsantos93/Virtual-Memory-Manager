/*
* CESAR School
* Computer Science @ Software Insfrastructure (Operating Systems)
* Professor Erico Teixeira
* Group 02: Arthur Carlos, João Victor Pessoa, Jonathan Coutinho,
*           Leonardo Melo and Pedro Henrique
* Reference: Operating System Concepts 10th Edition (Chapter 10),
*            by Abraham Silberschatz et al
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "TLB.c"

/* Specifics. */
#define PAGE_TABLE_ENTRIES       256
#define PAGE_SIZE_IN_BYTES       256
#define PAGE_NUMBER_SIZE_BITS    8
#define FRAME_AMOUNT             256 // 128
#define FRAME_SIZE_IN_BYTES      256
#define TLB_ENTRIES              16
#define PHYSICAL_MEM_SIZE        (FRAME_AMOUNT * FRAME_SIZE_IN_BYTES)

/* Macros for bit-masking and bit-shifting. */
#define PAGE_NUMBER_MASK      0xff00    /* keep bits from [15:8].  2^8-1.               */
#define OFFSET_NUMBER_MASK    0xff      /* 0xff = 255 =  2^8-  keep bits from [15:8].1. */
#define GET_PAGE_NUMBER(logicalAddres)                  (((logicalAddres) & (PAGE_NUMBER_MASK)) >> PAGE_NUMBER_SIZE_BITS)
#define GET_OFFSET_NUMBER(logicalAddres)                ((logicalAddres) & (OFFSET_NUMBER_MASK))
#define GET_PHYSICAL_ADDRESS(frameNumber, offsetNumber) ((frameNumber << PAGE_NUMBER_SIZE_BITS) | (offsetNumber))

/* Data Structures. */
int pageTable [PAGE_TABLE_ENTRIES];
int physicalMemory [PHYSICAL_MEM_SIZE];

/* File pointers. */
FILE *addresses;
FILE *backing_store;

/* Statistics. */
int addressAccessCount = 0;
int pageFaultCount = 0;
int tlbHitCount = 0;

/* Prototype functions. */
int verifyPageTable(int pageNumber);
int findFreeFrame(void);
char getValueFromPhysicalMemory(int frameNumber, int offSetNumber);
char *getFrameFromBackStore(FILE *backing_store, int pageNumber);
void initializeArray(int *array, int size);

int main(int argc, const char *argv[])
{
  /* not enough parameters passed OR invalid page replacement algorithm choice. */
  if (argc != 4 || !(atoi(argv[3]) != 0 || atoi(argv[3]) != 1)) {
    printf("Error: to run the program, use: ./a.out <path/adresses.txt> <path/BACKING_STORE.bin> <0: fifo OR 1:lru>");
    return -1;
  }

  addresses = fopen(argv[1], "r");
  backing_store = fopen(argv[2], "rb");
  int pageReplacementAlgorithm = atoi(argv[3]);

  /* initialize data structures. */
  TLB tlb = initialize();
  initializeArray(physicalMemory, PHYSICAL_MEM_SIZE);

  /* read through addresses.txt file, line-by-line. */
  int logicalAddress = 0;
  while(fscanf(addresses, "%d", &logicalAddress) == 1) {
    addressAccessCount++;

    int pageNumber = GET_PAGE_NUMBER(logicalAddress);
    int offSetNumber = GET_OFFSET_NUMBER(logicalAddress);
    int frameNumber = -1;

    /* consulta na TLB e na Page Table */
    TLB_Entry *tlb_entry = query(tlb, pageNumber);
    // case: ESTA na TLB.
    if (tlb_entry != NULL) {
      tlbHitCount++;
      frameNumber = tlb_entry->frameNumber;
    // case: NAO ESTA na TLB.
    } else {
      frameNumber = verifyPageTable(pageNumber);
      if(frameNumber == -1 ) {
        // char  page[PAGE_SIZE_IN_BYTES] = getFrameFromBackStore(pageNumber);
        char *page = getFrameFromBackStore(backing_store, pageNumber);
        int frameNumber = findFreeFrame();

        for(int i = frameNumber; i <(frameNumber +FRAME_SIZE_IN_BYTES); i++)
          physicalMemory[i] = page[i];

        pageTable[pageNumber] = frameNumber;              // atualiza PageTable
      }

      /* salvar na TLB. */
      TLB_Entry *newEntry = malloc(sizeof(TLB_Entry));
      newEntry->frameNumber =frameNumber;
      newEntry->pageNumber = pageNumber;
      newEntry->isFree = 1;
      newEntry->entryAge = 0;

      insertEntry(&tlb, newEntry, pageReplacementAlgorithm);
    }

    int physicalAddress = GET_PHYSICAL_ADDRESS(frameNumber, offSetNumber);
    char value = getValueFromPhysicalMemory(pageNumber, offSetNumber);
    char value02 = getValueFromPhysicalMemory(physicalAddress, offSetNumber);
    char value03 = getValueFromPhysicalMemory((char)(physicalAddress), offSetNumber);

    printf("Virtual Address: %d ", frameNumber);
    printf("Physical Address01: %d ", pageNumber);
    printf("Physical Address02: %d ", physicalAddress);
    printf("Physical Address03: %d ", (char)(physicalAddress));
    printf("Value01: %d ", value);
    printf("Value02: %d ", value02);
    printf("Value04: %d \n", value03);
  }

  printf("Page Fault Rate: %f\n", (float)(pageFaultCount/addressAccessCount));
  printf("TLB Hit Rate: %f\n", (float)(tlbHitCount/addressAccessCount));

  /* close all files. */
  fclose(addresses);
  fclose(backing_store);

  return 0;
}

/* Takes a page number and verifies pageTable for frame number. */
int verifyPageTable(int pageNumber)
{
  for(int index = 0; index < 256; index++) {
    if(pageTable[pageNumber])
      return pageTable[pageNumber];
  }

  return -1;
}

int findFreeFrame(void)
{
  for(int index = 0; index < PHYSICAL_MEM_SIZE; index + PAGE_SIZE_IN_BYTES) {
    if(physicalMemory[index] == (char)(-1)) // TO DO: Fix this comparison... before: '-1'
      return index;
  }

  return -1;
}

char getValueFromPhysicalMemory(int frameNumber, int offSetNumber)
{
  if(physicalMemory[frameNumber + offSetNumber])
    return physicalMemory[frameNumber + offSetNumber];

  return -1;
}

char *getFrameFromBackStore(FILE *backing_store, int pageNumber)
{
  char frame[FRAME_SIZE_IN_BYTES];
  fseek(backing_store, FRAME_SIZE_IN_BYTES *pageNumber, SEEK_SET);
  fread(backing_store, FRAME_SIZE_IN_BYTES, 1, backing_store);

  return frame;
}

void initializeArray(int *array, int size)
{
  for(int index = 0; index < size; index++)
    array[index] = -1;
}
