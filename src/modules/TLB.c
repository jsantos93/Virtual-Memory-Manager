#include <stdlib.h>

#define TLB_ENTRIES 16
#define FIFO_MODE 0
#define LRU_MODE 1

typedef struct tlb_entry
{
    int pageNumber;
    int frameNumber;
    int isFree;
    int entryAge;
    struct tlb_entry *next;
} TLB_Entry;

typedef struct tlb
{
    int tlb_len;
    int curr_len;
    int insertionMode;
    TLB_Entry *head;
    TLB_Entry *last;
} TLB;

TLB initialize(int insertionMode)
{
    TLB initialized;

    initialized.curr_len = 0;
    initialized.tlb_len = TLB_ENTRIES;
    initialized.insertionMode = insertionMode;

    return initialized;
}

TLB_Entry *query(TLB table, int pageNumber)
{
    TLB_Entry *current = table.head;

    while (current != NULL)
    {
        if (current->pageNumber == pageNumber)
        {
            return current;
        }

        current->entryAge++;
        current = current->next;
    }

    return NULL;
}

void lruInsertion(TLB *table, TLB_Entry *entry)
{
    /* find replacement entry. */
    TLB_Entry *current = table->head;
    for (int index = 0; index < table->tlb_len; index++)
    {
        // case: entry IS in TLB
        if (current->pageNumber == entry->pageNumber)
        {
            current->entryAge = 0;
            return;
            // case: entry IS NOT in TLB
        }
        else if (entry->pageNumber != current->pageNumber)
        {
            if (current->isFree)
            {
                entry->isFree = 0;
                entry->entryAge = 0;
                entry->next = current->next;
                current = entry;
                return;
            }
            else if (!current->isFree)
            {
                current->entryAge++;
            }
        }

        current = current->next;
    }

    /* case: entry IS NOT in TLB && no free spots found (must find oldest entry). */
    TLB_Entry *replacement = table->head;
    TLB_Entry *next = replacement->next;
    for (int index = 0; index < table->tlb_len; index++)
    {
        if (next->entryAge > replacement->entryAge)
        {
            entry->isFree = 0;
            entry->entryAge = 0;
            entry->next = next;
            replacement = entry;
            return;
        }

        replacement = replacement->next;
        next = replacement->next;
    }

    return;
}

void insertEntry(TLB *table, TLB_Entry *entry)
{
    if (table->last == table->head)
    {
        table->head = entry;
        table->last = entry;
    }
    else if (table->curr_len == table->tlb_len - 1)
    {
        switch (table->insertionMode)
        {
        case FIFO_MODE:
            table->head = entry;
            table->last = entry;
            break;
        case LRU_MODE:
            lruInsertion(table, entry);
            break;
        }
    }
    else
    {
        table->last->next = entry;
    }

    table->curr_len++;
}
