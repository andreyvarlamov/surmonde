#ifndef INVENTORY_H
#define INVENTORY_H

#include "sav.h"
#include "inventory_item.h"

struct InventoryBlock;

struct InventoryStore
{
    int itemIdSeed;
    int blockCount;
    int itemsPerBlock;
    InventoryBlock *blocks;
    InventoryBlock *free;
};

struct Entity;

struct InventoryBlock
{
    InventoryItem items[ITEMS_PER_INVENTORY_BLOCK];
    int itemCount;
    InventoryBlock *next;

    // Maybe
    Entity *entity;
};

api_func InventoryStore MakeInventoryStore(int blockCount, MemoryArena *arena);
api_func InventoryBlock *GetFreeInventoryBlock(InventoryStore *s, Entity *entityReference);
/* api_func void FreeInventoryBlock(InventoryStore *s, InventoryBlock *chainStart, InventoryBlock *block); */
api_func void FreeInventoryBlockChain(InventoryStore *s, InventoryBlock *chainStart);

api_func InventoryItem *FindItemInInventoryById(InventoryBlock *headBlock, int id);
api_func void AddItemToInventory(InventoryStore *s, InventoryBlock **headBlock, InventoryItem item, Entity *entityReference);
api_func void RemoveItemFromInventory(InventoryStore *s, InventoryBlock **headBlock, InventoryItem *item);

struct InventoryItemIterator
{
    int globalIndex;
    int indexInBlock;
    InventoryBlock *block;
};
api_func InventoryItemIterator GetInventoryItemIterator(InventoryStore *s, InventoryBlock *block);
api_func InventoryItem *NextInventoryItem(InventoryItemIterator *iterator);

#endif
