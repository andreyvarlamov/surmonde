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
    int segments,
    MemoryArena *arena);

#endif