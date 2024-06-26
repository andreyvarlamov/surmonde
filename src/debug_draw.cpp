#include "debug_draw.h"

struct DDLine
{
    DDLine *next;
    
    v2 start, end;
    SavColor color;
};

struct DDPoint
{
    DDPoint *next;

    v2 point;
    SavColor color;
};

struct DebugDrawState
{
    MemoryArena *arena;
    DDLine *lines;
    DDPoint *points;
    int lineCount;
    int pointCount;
};

global_var DebugDrawState *_ddState;

api_func void DDrawInit(MemoryArena *arena)
{
    _ddState = MemoryArenaPushStruct(arena, DebugDrawState);
    _ddState->arena = arena;

    MemoryArenaFreeze(arena);
}

api_func void DDrawPoint(v2 p, SavColor color)
{
    DDPoint *point = MemoryArenaPushStruct(_ddState->arena, DDPoint);
    point->point = p;
    point->color = color;
    point->next = _ddState->points;
    _ddState->points = point;
    _ddState->pointCount++;
}

api_func void DDrawLine(v2 start, v2 end, SavColor color)
{
    DDLine *line = MemoryArenaPushStruct(_ddState->arena, DDLine);
    line->start = start;
    line->end = end;
    line->color = color;
    line->next = _ddState->lines;
    _ddState->lines = line;
    _ddState->lineCount++;
}

internal_func void drawLines()
{
    int vertCount = _ddState->lineCount * 2;
    int indexCount = vertCount;

    v3 *positions = MemoryArenaPushArrayAndZero(_ddState->arena, vertCount, v3);
    v4 *colors = MemoryArenaPushArrayAndZero(_ddState->arena, vertCount, v4);
    u32 *indices = MemoryArenaPushArrayAndZero(_ddState->arena, indexCount, u32);

    int vertsAdded = 0;
    int indicesAdded = 0;

    DDLine *line = _ddState->lines;
    while (line != NULL)
    {
        positions[vertsAdded] = V3(line->start);
        colors[vertsAdded] = GetColorV4(line->color);
        indices[indicesAdded++] = vertsAdded;
        vertsAdded++;

        positions[vertsAdded] = V3(line->end);
        colors[vertsAdded] = GetColorV4(line->color);
        indices[indicesAdded++] = vertsAdded;
        vertsAdded++;

        line = line->next;
    }

    Assert(vertsAdded == vertCount);
    Assert(indicesAdded == indexCount);
    VertexBatchBeginSub(DEFAULT_VERTEX_BATCH, vertCount, indexCount);
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_POSITIONS, MakeVertexCountedData(positions, vertCount, sizeof(positions[0])));
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_COLORS, MakeVertexCountedData(colors, vertCount, sizeof(colors[0])));
    VertexBatchSubIndexData(DEFAULT_VERTEX_BATCH, MakeVertexCountedData(indices, indexCount, sizeof(indices[0])));
    VertexBatchEndSub(DEFAULT_VERTEX_BATCH);

    SetLineWidth(10.0f);
    SetDrawMode(SAV_DRAW_LINES);
    DrawVertexBatch(DEFAULT_VERTEX_BATCH);
    SetDrawMode(SAV_DRAW_TRIANGLES);
    SetLineWidth(1.0f);
}

internal_func void drawPoints()
{
    int vertCount = _ddState->pointCount;
    int indexCount = vertCount;

    v3 *positions = MemoryArenaPushArrayAndZero(_ddState->arena, vertCount, v3);
    v4 *colors = MemoryArenaPushArrayAndZero(_ddState->arena, vertCount, v4);
    u32 *indices = MemoryArenaPushArrayAndZero(_ddState->arena, indexCount, u32);

    int vertsAdded = 0;
    int indicesAdded = 0;

    DDPoint *point = _ddState->points;
    while (point != NULL)
    {
        positions[vertsAdded] = V3(point->point);
        colors[vertsAdded] = GetColorV4(point->color);
        indices[indicesAdded++] = vertsAdded;
        vertsAdded++;

        point = point->next;
    }

    Assert(vertsAdded == vertCount);
    Assert(indicesAdded == indexCount);
    VertexBatchBeginSub(DEFAULT_VERTEX_BATCH, vertCount, indexCount);
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_POSITIONS, MakeVertexCountedData(positions, vertCount, sizeof(positions[0])));
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_COLORS, MakeVertexCountedData(colors, vertCount, sizeof(colors[0])));
    VertexBatchSubIndexData(DEFAULT_VERTEX_BATCH, MakeVertexCountedData(indices, indexCount, sizeof(indices[0])));
    VertexBatchEndSub(DEFAULT_VERTEX_BATCH);

    SetPointSize(10.0f);
    SetDrawMode(SAV_DRAW_POINTS);
    DrawVertexBatch(DEFAULT_VERTEX_BATCH);
    SetDrawMode(SAV_DRAW_TRIANGLES);
    SetPointSize(1.0f);
}

api_func void DDraw()
{
    if (_ddState->lineCount > 0)
    {
        drawLines();
    }
    if (_ddState->pointCount > 0)
    {
        drawPoints();
    }

    MemoryArenaRevert(_ddState->arena);

    _ddState->lines = NULL;
    _ddState->points = NULL;
    _ddState->lineCount = 0;
    _ddState->pointCount = 0;
    
}
