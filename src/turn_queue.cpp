#include "turn_queue.h"
#include "sav.h"

internal_func TurnQueueNode *getFreeNode(TurnQueue *q)
{
    if (q->freeNode)
    {
        TurnQueueNode *freeNode = q->freeNode;
        q->freeNode = q->freeNode->next;
        *freeNode = {};
        return freeNode;
    }
    else
    {
        TurnQueueNode *freeNode = MemoryArenaPushStructAndZero(q->arena, TurnQueueNode);
        return freeNode;
    }
}

internal_func void removeNode(TurnQueue *q, TurnQueueNode *nodeToRemove, TurnQueueNode *previous)
{
    if (previous != NULL)
    {
        previous->next = nodeToRemove->next;
    }
    else
    {
        q->nextNode = nodeToRemove->next;
    }

    nodeToRemove->next = q->freeNode;
    q->freeNode = nodeToRemove;
}

internal_func void insertNodeAtTime(TurnQueue *q, TurnQueueNodeType t, Entity *e, i64 time)
{
    TurnQueueNode *previous = NULL;
    TurnQueueNode *current = q->nextNode;
    while (current)
    {
        if (current->time > time)
        {
            break;
        }
        previous = current;
        current = current->next;
    }

    TurnQueueNode *newNode = getFreeNode(q);
    newNode->t = t;
    newNode->e = e;
    newNode->next = current;
    if (previous != NULL)
    {
        previous->next = newNode;
    }
    else
    {
        q->nextNode = newNode;
    }
}

api_func TurnQueue MakeTurnQueue(MemoryArena *arena)
{
    TurnQueue q;
    q.nextNode = NULL;
    q.freeNode = NULL;
    q.arena = arena;
    insertNodeAtTime(&q, TURN_QUEUE_TURN_MARKER, NULL, 0);
    return q;
}

api_func TurnQueueNode TurnQueuePeek(TurnQueue *q)
{
    Assert(q->nextNode != NULL);
    TurnQueueNode result = *(q->nextNode);
    result.next = NULL;
    return result;
}

api_func TurnQueueNode TurnQueuePop(TurnQueue *q)
{
    TurnQueueNode *poppedNode = q->nextNode;
    Assert(poppedNode != NULL);

    TurnQueueNode result = *poppedNode;
    result.next = NULL;

    removeNode(q, poppedNode, NULL);

    if (result.t == TURN_QUEUE_TURN_MARKER)
    {
        insertNodeAtTime(q, TURN_QUEUE_TURN_MARKER, NULL, result.time + 100);
    }

    return result;
}

api_func void TurnQueueInsertEntityAction(TurnQueue *q, Entity *e, i64 time)
{
    insertNodeAtTime(q, TURN_QUEUE_ACTION, e, time);
}

api_func void TurnQueueRemoveAllActionsForEntity(TurnQueue *q, Entity *e)
{
    TurnQueueNode *previous = NULL;
    TurnQueueNode *current = q->nextNode;
    while (current)
    {
        if (current->e == e)
        {
            removeNode(q, current, previous);
        }
        previous = current;
        current = current->next;
    }
}
