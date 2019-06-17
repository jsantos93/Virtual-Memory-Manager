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
#include <sys/types.h>

/* Specifics. */
#define PAGE_TABLE_ENTRIES    256
#define PAGE_SIZE_IN_BYTES    256
#define FRAME_AMOUNT          128
#define FRAME_SIZE_IN_BYTES   256
#define TLB_ENTRIES           16
#define PHYSICAL_MEM_SIZE     (FRAME_AMOUNT * FRAME_SIZE_IN_BYTES)

/* Macros for bit-masking and bit-shifting. */
#define PAGE_NUMBER_MASK      0xff00    // keep bits from [15:8]
#define OFFSET_NUMBER_MASK    0xff      // keep bits from [7:0]
#define GET_PAGE_NUMBER(logicalAddres) (((logicalAddres) & (PAGE_NUMBER_MASK)) >> 8)
#define GET_OFFSET_NUMBER(logicalAddres) ((logicalAddres) & (OFFSET_NUMBER_MASK))
#define GET_PHYSICAL_ADDRESS(frameNumber, offsetNumber) ((frameNumber << 8) | (offsetNumber))

/* Data Structures. */
typedef struct Page {
  unsigned char isValid;
  unsigned int pageNumber;
  unsigned int frameNumber;
} Page;

typedef struct Frame {
  unsigned char isFree;
  unsigned char bytes[FRAME_SIZE_IN_BYTES];
} Frame;

typedef struct Tlb {
  Page pages[TLB_ENTRIES];
} Tlb;

Page pageTable[PAGE_TABLE_ENTRIES];
Frame physicalMemory[FRAME_AMOUNT];
Tlb tlb[TLB_ENTRIES];

/* Statistics. */
int addressesReferenced = 0;
int pageFaultCounter = 0;
int tlbHitCounter = 0;

/* Prototype functions. */
int readInputFile(void);
int verifyTlb(void);
int verifyPageTable(void);
int updateTlb(void);
int updatePageTable(void);
int fifo(void);
int lru(void);
int calculatePageFaultRate(void);
int calculateTlbHitRate(void);

int main(int argc, const char * argv[])
{
  /* do some work here */

  return 0;
}

/* Reads file and returns a 32-bit integer (logical adress). */
int readInputFile(void)
{
  /* do some work here */

  /*
   * Na subseção 'Specific' (pg. 566) diz:
   * "[..] However, you need only be concerned with 16- bit addresses,
   * so you must mask the rightmost 16 bits of each logical address."
   *
   * "mask" como? adicionando 0 à esquerda (shift), por exemplo?
   * "mask" onde? vai ser aqui, em getPageNumer/Offset,  ou outro lugar?
  */

  return 0;
}

/* Takes a page number and verifies TLB for frame number. */
int verifyTlb(void)
{
  /* do some work here */

  return 0;
}

/* Takes a page number and verifies pageTable for frame number. */
int verifyPageTable(void)
{
  /* do some work here */

  return 0;
}

/* Takes a frame number and updates the TLB. */
int updateTlb(void)
{
  /* do some work here */

  return 0;
}

/* Takes a frame number and updates the pageTable. */
int updatePageTable(void)
{
  /* do some work here */

  return 0;
}

/* Applies the FIFO algorithm as page-replacement policy when there is no free memory. */
int fifo(void)
{
  /* do some work here. */

  return 0;
}

/* Applies the LRU algorithm as page-replacement policy when there is no free memory. */
int lru(void)
{
  /* do some work here. */

  return 0;
}

/* Calculates the percentage of address references that resulted in page faults. */
int calculatePageFaultRate(void)
{
  /* do some work here. */

  return 0;
}

/* Calculates the percentage of address references that were resolved in the TLB. */
int calculateTlbHitRate(void)
{
  /* do some work here. */

  return 0;
}
