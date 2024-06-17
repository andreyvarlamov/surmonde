#ifndef DRAW_H
#define DRAW_H

api_func void DrawAtlasTilemap(
    SavTextureAtlas atlas,
    int tilemapWidth,
    int tilemapHeight,
    f32 tilePxW,
    f32 tilePxH,
    i32 *atlasValues,
    SavColor *colorsFg,
    SavColor *colorsBg,
    v2 origin,
    MemoryArena *arena);

api_func void DrawFilledCircle(
    v2 position,
    f32 radius,
    SavColor color,
    MemoryArena *arena,
    int segments = 16);

api_func void DrawFilledCircleSegment(
    v2 center,
    f32 radius,
    f32 startDeg,
    f32 endDeg,
    SavColor color,
    int segments,
    MemoryArena *arena);

api_func void DrawFilledCircleSegment(
    v2 center,
    f32 radius,
    f32 startDeg,
    f32 endDeg,
    SavColor color,
    MemoryArena *arena);

api_func void DrawLine(
    v2 start,
    v2 end,
    SavColor color);

struct Floater
{
    b32 isUsed;
    const char *string; // TODO: Should not be alloced on stack
    SavFont *font;
    f32 pointSize;
    SavColor color;
    v2 startingWorldP;
    
    f32 yOffset;
    f32 opacity;
};

api_func void InitializeFloaterState(SavFont *font, Camera2D *camera, MemoryArena *arena);
api_func void AddFloater(const char *string, f32 pointSize, SavColor color, v2 worldP);
api_func void DrawFloaters(f32 delta);

#endif
