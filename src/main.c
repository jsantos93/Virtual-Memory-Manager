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
#define FRAME_AMOUNT             128
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
signed char page   [PAGE_SIZE_IN_BYTES];
int pageTable      [PAGE_TABLE_ENTRIES];
int physicalMemory [FRAME_AMOUNT][FRAME_SIZE_IN_BYTES];

/* File pointers. */
FILE *addresses;
FILE *backing_store;
FILE *output;

/* Statistics. */
int addressAccessCount = 0;
int pageFaultCount = 0;
int tlbHitCount = 0;

/* Prototype functions. */
int verifyPageTable(int pageNumber);
int insertFromBackStore(int pageNumber);
char *getFrameFromBackStore(int pageNumber);

int main(int argc, const char *argv[])
{
  /* not enough parameters passed OR invalid page replacement algorithm choice. */
  if (argc != 4 || !(atoi(argv[3]) != 0 || atoi(argv[3]) != 1)) {
    printf("Error: to run the program, use: ./a.out <path/adresses.txt> <path/BACKING_STORE.bin> <0: fifo OR 1:lru>");
    return -1;
  }

  output = fopen("output.txt", "w");
  addresses = fopen(argv[1], "r");
  backing_store = fopen(argv[2], "rb");
  int pageReplacementAlgorithm = atoi(argv[3]);

  /* initialize data structures. */
  TLB tlb = initialize();
  memset(physicalMemory, -1, sizeof(physicalMemory[0][0]) * FRAME_AMOUNT * FRAME_SIZE_IN_BYTES);

  /* read through addresses.txt file, line-by-line. */
  int logicalAddress = 0;
  while(fscanf(addresses, "%d", &logicalAddress) == 1) {
    addressAccessCount++;

    int pageNumber = GET_PAGE_NUMBER(logicalAddress);
    int offsetNumber = GET_OFFSET_NUMBER(logicalAddress);
    int frameNumber = -1;

    /* consulta na TLB e na Page Table */
    TLB_Entry *tlb_entry = query(tlb, pageNumber);

    // case: verificar se ESTA na TLB.
    if (tlb_entry != NULL) {
      tlbHitCount++;
      frameNumber = tlb_entry->frameNumber;
    }

    if(frameNumber == -1 ) {
      // case: verificar se ESTA na PageTable.
      int pageTableFrameNumber = verifyPageTable(pageNumber);
      frameNumber = (pageTableFrameNumber == -1) ? insertFromBackStore(pageNumber) : pageTableFrameNumber;
      pageFaultCount++;
    }

    /* inserir frame e page number na TLB. */
    TLB_Entry *newEntry = malloc(sizeof(TLB_Entry));
    newEntry->frameNumber =frameNumber;
    newEntry->pageNumber = pageNumber;
    newEntry->isFree = 1;
    newEntry->entryAge = 0;
    insertEntry(&tlb, newEntry, pageReplacementAlgorithm);

    /* inserir frame e page number na TLB. */
    int physicalAddress = GET_PHYSICAL_ADDRESS(frameNumber, offsetNumber);
    signed char value = physicalMemory[frameNumber][offsetNumber];

    printf("Virtual Address: %d\t", logicalAddress);
    printf("Physical Address: %d\t", physicalAddress);
    printf("Value: %d\n", value);
  }

  /* print statics */
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
  for(int pageEntry = 0; pageEntry < PAGE_TABLE_ENTRIES; pageEntry++) {
    if(pageTable[pageEntry] == pageNumber || pageTable[pageEntry] == -1)
      return pageTable[pageEntry];
    else if(pageTable[pageNumber] == pageNumber)
      return pageTable[pageNumber];
  }

  return -1;
}

char *getFrameFromBackStore(int pageNumber)
{
  fseek(backing_store, (PAGE_SIZE_IN_BYTES * pageNumber), SEEK_SET);
  fread(page, PAGE_SIZE_IN_BYTES, sizeof(signed char), backing_store);

  return page;
}

int insertFromBackStore(int pageNumber)
{
  char *page = getFrameFromBackStore(pageNumber);

  for(int frameNumber = 0; frameNumber < FRAME_AMOUNT; frameNumber++) {
    for(int frameByte = 0; frameByte < FRAME_SIZE_IN_BYTES; frameByte++) {
      if(physicalMemory[frameNumber][frameByte] == -1) {
        physicalMemory[frameNumber][frameByte] = page[frameByte];
        pageTable[pageNumber] = frameNumber;
      }
    }
  }

  return -1;
}
