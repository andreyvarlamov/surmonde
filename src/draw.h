#ifndef DRAW_H
#define DRAW_H

api_func void DrawAtlasTilemap(
    SavTextureAtlas atlas,
    int tilemapWidth,
    int tilemapHeight,
    f32 tilePxW,
    f32 tilePxH,
    i32 *atlasValues,
    v4 *colors,
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

api_func void DrawBadLine(
    v2 start,
    v2 end,
    SavColor color);

api_func void DrawFilledPolygon(v2 *verts, int vertCount, SavColor color);

struct Floater
{
    b32 isUsed;
    char string[256];
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
