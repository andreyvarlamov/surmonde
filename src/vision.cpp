#include "sav.h"
#include "vision.h"
#include "level.h"
#include "entity.h"

internal_func void traceLine(Level *level, EntityStore *entityStore, v2i from, i32 x1, i32 y1, i32 rangeLimitSq, u8 *visibleTiles)
{
    i32 x0 = from.x; i32 y0 = from.y;
    
    i32 dx = AbsI32(x1 - x0);
    i32 dy = -AbsI32(y1 - y0);
    i32 e = dx + dy;

    i32 sx = -1; if (x0 < x1) sx = 1;
    i32 sy = -1; if (y0 < y1) sy = 1;

    visibleTiles[VecToIdx(level->w, from)] = true;

    while (true)
    {
        visibleTiles[XYToIdx(level->w, x0, y0)] = true;
        
        if (AnyOpaqueEntityAtTile(entityStore, V2I(x0, y0))) return;
        if (IsTileOpaque(level, x0, y0)) return;
        if (x0 == x1 && y0 == y1) return;
        if (Square(x0 - from.x) + Square(y0 - from.y) >= rangeLimitSq) return;
        
        i32 e2 = 2 * e;
        if (e2 >= dy) { if (x0 == x1) return; e += dy; x0 += sx; }
        if (e2 <= dx) { if (y0 == y1) return; e += dx; y0 += sy; }
    }
}

internal_func b32 canTraceLine(Level *level, EntityStore *entityStore, v2i from, i32 x1, i32 y1, i32 rangeLimitSq)
{
    i32 x0 = from.x; i32 y0 = from.y;
    
    i32 dx = AbsI32(x1 - x0);
    i32 dy = -AbsI32(y1 - y0);
    i32 e = dx + dy;

    i32 sx = -1; if (x0 < x1) sx = 1;
    i32 sy = -1; if (y0 < y1) sy = 1;

    while (true)
    {
        if (AnyOpaqueEntityAtTile(entityStore, V2I(x0, y0))) return false;
        if (IsTileOpaque(level, x0, y0)) return false;
        if (x0 == x1 && y0 == y1) return true;
        if (Square(x0 - from.x) + Square(y0 - from.y) >= rangeLimitSq) return false;
        
        i32 e2 = 2 * e;
        if (e2 >= dy) { if (x0 == x1) return true; e += dy; x0 += sx; }
        if (e2 <= dx) { if (y0 == y1) return true; e += dx; y0 += sy; }
    }
}

api_func b32 IsInLineOfSight(Level *level, EntityStore *entityStore, v2i from, v2i target, i32 rangeLimit)
{
    return canTraceLine(level, entityStore, from, target.x, target.y, Square(rangeLimit));
}

api_func b32 IsInLineOfSight(Level *level, EntityStore *entityStore, v2 from, v2 target, i32 rangeLimit)
{
    v2i fromTile = GetTilePFromFloatP(from);
    v2i targetTile = GetTilePFromFloatP(target);
    return canTraceLine(level, entityStore, fromTile, targetTile.x, targetTile.y, Square(rangeLimit));
}

api_func void CalculateVision(Level *level, EntityStore *entityStore, v2i p, i32 rangeLimit, u8 *visibleTiles)
{
    Memset(visibleTiles, 0, level->w * level->h);
    
    visibleTiles[VecToIdx(level->w, p)] = true;

    if (rangeLimit > 0)
    {
        RectInt mapRect = MakeRect(0, 0, level->w, level->h);
        RectInt rangeLimitRect = MakeRect(p.x - rangeLimit, p.y - rangeLimit, rangeLimit * 2 + 1, rangeLimit * 2 + 1);
        RectInt visionRect = RectIntersect(mapRect, rangeLimitRect);

        v2i max = RectGetMax(visionRect);
        i32 rangeLimitSq = Square(rangeLimit);
        
        for (i32 x = visionRect.x; x < max.x; x++)
        {
            traceLine(level, entityStore, p, x, visionRect.y, rangeLimitSq, visibleTiles);
            traceLine(level, entityStore, p, x, max.y - 1,    rangeLimitSq, visibleTiles);
        }

        for (i32 y = visionRect.y; y < max.y; y++)
        {
            traceLine(level, entityStore, p, visionRect.x, y, rangeLimitSq, visibleTiles);
            traceLine(level, entityStore, p, max.x - 1,    y, rangeLimitSq, visibleTiles);
        }
    }
}
