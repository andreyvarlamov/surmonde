#include "inventory.h"

#include "sav.h"

api_func InventoryStore MakeInventoryStore(int blockCount, MemoryArena *arena)
{
    InventoryStore s;
    s.blockCount = blockCount;
    s.itemsPerBlock = ITEMS_PER_INVENTORY_BLOCK;
    s.blocks = MemoryArenaPushArrayAndZero(arena, s.blockCount, InventoryBlock);
    for (int i = s.blockCount - 2; i >= 0; i--)
    {
        s.blocks[i].next = s.blocks + i + 1;
    }
    s.free = s.blocks;
    return s;
}

struct Entity;
api_func InventoryBlock *GetFreeInventoryBlock(InventoryStore *s, Entity *entityReference)
{
    Assert(s->free);
    InventoryBlock *free = s->free;
    s->free = free->next;
    free->next = NULL;
    free->entity = entityReference;
    return free;
}

api_func void FreeInventoryBlockChain(InventoryStore *s, InventoryBlock *headBlock)
{
    InventoryBlock *last = NULL;
    for (InventoryBlock *block = headBlock; block != NULL; block = block->next)
    {
        block->entity = NULL;
        last = block;
    }
    last->next = s->free;
    s->free = last;
}

api_func void AddItemToInventory(InventoryStore *s, InventoryBlock **headBlock, InventoryItem item, Entity *entityReference)
{
    InventoryBlock *last = NULL;
    InventoryItem *itemSlot = NULL;
    for (InventoryBlock *block = *headBlock; block != NULL; block = block->next)
    {
        if (block->itemCount < s->itemsPerBlock)
        {
            itemSlot = &block->items[block->itemCount++];
            break;
        }
        last = block;
    }
    if (itemSlot == NULL)
    {
        if (last == NULL)
        {
            *headBlock = GetFreeInventoryBlock(s, entityReference);
            itemSlot = (*headBlock)->items;
            (*headBlock)->itemCount++;
        }
        else
        {
            last->next = GetFreeInventoryBlock(s, entityReference);
            itemSlot = last->next->items;
            last->next->itemCount++;
        }
    }
    *itemSlot = item;
}

struct FindItemInInventoryResult
{
    InventoryBlock *block;
    int indexInBlock;
};

internal_func FindItemInInventoryResult findItemInInventory(InventoryBlock *headBlock, InventoryItem *item)
{
    FindItemInInventoryResult result = {};
    for (InventoryBlock *block = headBlock; block != NULL; block = block->next)
    {
        for (int i = 0; i < block->itemCount; i++)
        {
            if (item == block->items + i)
            {
                result.block = block;
                result.indexInBlock = i;
                return result;
            }
        }
    }
    return result;
}

api_func void RemoveItemFromInventory(InventoryStore *s, InventoryBlock **headBlock, InventoryItem *item)
{
    FindItemInInventoryResult foundItem = findItemInInventory(*headBlock, item);
    Assert(foundItem.block != NULL);

    InventoryBlock *lastLast = NULL;
    InventoryBlock *last = NULL;
    for (InventoryBlock *block = *headBlock; block != NULL; block = block->next)
    {
        lastLast = last;
        last = block;
    }
    int lastItemIndex = --last->itemCount;
    InventoryItem *lastItem = last->items + lastItemIndex;
    
    foundItem.block->items[foundItem.indexInBlock] = *lastItem;

    if (last->itemCount == 0)
    {
        if (lastLast != NULL)
        {
            lastLast->next = NULL;
        }
        else
        {
            *headBlock = NULL;
        }

        last->entity = NULL;
        last->next = s->free;
        s->free = last;
    }
}

api_func InventoryItemIterator GetInventoryItemIterator(InventoryStore *s, InventoryBlock *block)
{
    InventoryItemIterator iterator;
    iterator.globalIndex = -1;
    iterator.indexInBlock = -1;
    iterator.block = block;
    return iterator;
}

api_func InventoryItem *NextInventoryItem(InventoryItemIterator *iterator)
{
    if (iterator->block == NULL)
    {
        return NULL;
    }
    
    iterator->indexInBlock++;
    iterator->globalIndex++;
    if (iterator->indexInBlock >= iterator->block->itemCount)
    {
        iterator->block = iterator->block->next;
        iterator->indexInBlock = 0;
    }

    InventoryItem *item = NULL;
    if (iterator->block != NULL)
    {
        item = iterator->block->items + iterator->indexInBlock;
    }
    return item;
}
