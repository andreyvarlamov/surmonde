#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include "sav.h"

enum TurnQueueNodeType
{
    TURN_QUEUE_ACTION,
    TURN_QUEUE_TURN_MARKER
};

struct Entity;
struct TurnQueueNode
{
    TurnQueueNodeType t;
    Entity *e;
    i64 time;

    TurnQueueNode *next;
};

struct TurnQueue
{
    TurnQueueNode *nextNode;
    TurnQueueNode *freeNode;
    MemoryArena *arena;
};

api_func TurnQueue MakeTurnQueue(MemoryArena *arena);
api_func TurnQueueNode TurnQueuePeek(TurnQueue *q);
api_func TurnQueueNode TurnQueuePop(TurnQueue *q);
api_func void TurnQueueInsertEntityAction(TurnQueue *q, Entity *e, i64 time);
api_func void TurnQueueRemoveAllActionsForEntity(TurnQueue *q, Entity *e);

#endif