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

/* Custom modules. */
#include "modules/TLB.c"

/* Specifics. */
#define PAGE_TABLE_ENTRIES     256
#define PAGE_SIZE_IN_BYTES     256
#define FRAME_AMOUNT           256 // Try 128 
#define FRAME_SIZE_IN_BYTES    256
#define TLB_ENTRIES            16
#define PHYSICAL_MEM_SIZE      (FRAME_AMOUNT * FRAME_SIZE_IN_BYTES)

/* Macros for bit-masking and bit-shifting. */
#define PAGE_NUMBER_MASK      0xff00    /* keep bits from [15:8].  2^8-1. */
#define OFFSET_NUMBER_MASK    0xff      /* 0xff = 255 =  2^8-  keep bits from [15:8].1. */
#define GET_PAGE_NUMBER(logicalAddres)                  (((logicalAddres) & (PAGE_NUMBER_MASK)) >> 8)
#define GET_OFFSET_NUMBER(logicalAddres)                ((logicalAddres) & (OFFSET_NUMBER_MASK))
#define GET_PHYSICAL_ADDRESS(frameNumber, offsetNumber) ((frameNumber << 8) | (offsetNumber))

/* Data Structures. */
int pageTable [PAGE_TABLE_ENTRIES];
int physicalMemory [PHYSICAL_MEM_SIZE];

/* Statistics. */
int addressAccessCount = 0;
int pageFaultCount = 0;
int tlbHitCount = 0;

/* Prototype functions. */
int verifyTlb(void);
int verifyPageTable(int pageNumber);
char getValueFromPhysicalMemory(int,int);
char * getFrameFromBackStore(int);
void initializeArray(int *arry, int size);
int findFreeFrame();
int main(int argc, const char * argv[])
{

  initializeArray(physicalMemory, PHYSICAL_MEM_SIZE);

  FILE *fp;
  fp = fopen("addresses.txt", "r");
  TLB tlb = initialize(FIFO_MODE);

  while(fp != EOF) {
    int logicalAddress;
    fscanf(fp, "%d", &logicalAddress);

    addressAccessCount++;
    int offSetNumber = GET_OFFSET_NUMBER(logicalAddress);
    int pageNumber = GET_PAGE_NUMBER(logicalAddress);
    int frameNumber = -1;
    char value;

    /*
     * consulta na TLB e na Page Table
     */

    TLB_Entry *tlb_entry = query(tlb, pageNumber);

    if (tlb_entry != NULL) {
      tlbHitCount++;
      frameNumber = tlb_entry->frameNumber;

    } else {
      //else nao esta na TLB

      frameNumber = verifyPageTable(pageNumber);

      if(frameNumber == -1 ){

        //pega do backing store
       // char  page[PAGE_SIZE_IN_BYTES] = getFrameFromBackStore(pageNumber);

        char *page = getFrameFromBackStore(pageNumber);


        //salva na memoria
        int frameNumber = findFreeFrame();
        for(int i = frameNumber; i <(frameNumber +FRAME_SIZE_IN_BYTES); i++){
          physicalMemory[i] = page[i];
        }

        // atualiza page Table
        pageTable[pageNumber] = frameNumber;

      }

      //salva na TLB
      TLB_Entry * newEntry = malloc(sizeof(TLB_Entry));
      newEntry->frameNumber =frameNumber;
      newEntry->pageNumber = pageNumber;
      newEntry->isFree = 1;
      newEntry->entryAge = 0;

      insertEntry(&tlb,newEntry);
    }

    value = getValueFromPhysicalMemory(pageNumber,offSetNumber);
    printf("Virtual Address: %d ", frameNumber);
    printf("Physical Address: %d ", pageNumber);
    printf("Value: %d /n", value);
  }

  printf("Page Fault Rate: %f/n", (float)pageFaultCount/addressAccessCount );
  printf("TLB Hit Rate: %f/n", (float)tlbHitCount/addressAccessCount);

  return 0;
}

/* Takes a page number and verifies pageTable for frame number. */
int verifyPageTable(int pageNumber)
{
  for(int i = 0; i < 256; i++) {
    if(pageTable[pageNumber] != NULL) {
      return pageTable[pageNumber];
    }
  }
  return -1;
}

int findFreeFrame(){
  for(int i = 0; i<PHYSICAL_MEM_SIZE;i+PAGE_SIZE_IN_BYTES){
    if(physicalMemory[i] == '-1'){
      return i;
    }
  }
  return -1;
}

char getValueFromPhysicalMemory(int frameNumber, int offSetNumber){

  if(physicalMemory[frameNumber + offSetNumber] != NULL){
    return physicalMemory[frameNumber + offSetNumber];
  }
  return -1;
}

char* getFrameFromBackStore(int pageNumber){
  char frame[FRAME_SIZE_IN_BYTES];
  FILE *ptr;
  ptr = fopen("BACKSTORE_BIN.bin","rb");
  fseek(ptr, FRAME_SIZE_IN_BYTES * pageNumber, SEEK_SET);
  fread(ptr, FRAME_SIZE_IN_BYTES,1,ptr);
  return frame;
}

void initializeArray(int *array, int size){
  for(int i=0; i < size; i++){
    array[i] = -1;
  }
}

// void updatePageTable(char* frame, char** page){
//   /*
//   * Pure Demand Paging: never bring a page into memory until it is required.
//   *
//   *  1. (10.2.2) free-frame list: a pool of free frames for satisfying such requests
//   *  2. (10.3) copy-on-write: if either process writes to a shared page, a copy of the shared page is created.
//   */
//   for(int i = 0; i < 256; i++){
//     page[i] =  page[i++];
//   }

//   page[255] = frame;
// }