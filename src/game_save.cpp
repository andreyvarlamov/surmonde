#include "game_save.h"

#include "sav.h"
#include "surmonde.h"

internal_func void serializeColor(SavFile file, v4 color, const char *prefix)
{
    SavFilePrintFormat(file, "%s_r: %0.5f\n", prefix, color.r);
    SavFilePrintFormat(file, "%s_g: %0.5f\n", prefix, color.g);
    SavFilePrintFormat(file, "%s_b: %0.5f\n", prefix, color.b);
    SavFilePrintFormat(file, "%s_a: %0.5f\n", prefix, color.a);
}

internal_func void serializeActorStats(SavFile file, ActorStats stats)
{
    SavFilePrintFormat(file, "view_radius: %0.5f\n", stats.viewRadius);
    SavFilePrintFormat(file, "attack_reach: %0.5f\n", stats.attackReach);
    SavFilePrintFormat(file, "speed: %0.5f\n", stats.speed);
    SavFilePrintFormat(file, "combat_radius: %0.5f\n", stats.combatRadius);
    SavFilePrintFormat(file, "health: %0.5f\n", stats.health);
    SavFilePrintFormat(file, "max_health: %0.5f\n", stats.maxHealth);
}

internal_func void serializeMachineData(SavFile file, EntityDataMachine *machineData)
{
    SavFilePrintFormat(file, "machine_type: %s\n", MachineTypeString[machineData->machineType]);
    SavFilePrintFormat(file, "begin processed_items_timers\n");
    for (int i = 0; i < machineData->processedItemCount; i++)
    {
        SavFilePrintFormat(file, "%d, %0.5f\n", machineData->processedItemIds[i], machineData->processedItemTimers[i]);
    }
    SavFilePrintFormat(file, "end processed_items_timers\n");
}

internal_func void serializeEntity(SavFile file, Entity *entity)
{
    SavFilePrintFormat(file, "begin entity\n");

    SavFilePrintFormat(file, "type: %s\n", EntityTypeString[entity->type]);
    SavFilePrintFormat(file, "p_x: %0.5f\n", entity->p.x);
    SavFilePrintFormat(file, "p_y: %0.5f\n", entity->p.y);
    SavFilePrintFormat(file, "yawDeg: %0.5f\n", entity->yawDeg);
    SavFilePrintFormat(file, "sprite_name: %s\n", SpriteAtlasNameString[entity->sprite.atlasName]);
    SavFilePrintFormat(file, "atlas_value: %d\n", entity->sprite.atlasValue);
    serializeColor(file, entity->color, "color");
    SavFilePrintFormat(file, "name: %s\n", entity->name);
    serializeActorStats(file, entity->stats);
    // TODO: ai state
    // TODO: actor order
    SavFilePrintFormat(file, "steer_target_active: %d\n", entity->steerTargetActive);
    SavFilePrintFormat(file, "steer_target: %0.5f\n", entity->steerTarget);
    serializeMachineData(file, &entity->machineData);
    SavFilePrintFormat(file, "is_paused: %d\n", entity->isPaused);
    SavFilePrintFormat(file, "is_blocking: %d\n", entity->isBlocking);
    SavFilePrintFormat(file, "is_opaque: %d\n", entity->isOpaque);
    
    SavFilePrintFormat(file, "end entity\n");
}

internal_func void serializeEntityStore(SavFile file, EntityStore *entityStore)
{
    SavFilePrintFormat(file, "begin entity_store\n");

    SavFilePrintFormat(file, "controlled_entity: %zu\n", entityStore->controlledEntity - entityStore->entities);

    for (int entityI = 0; entityI < entityStore->entityCount; entityI++)
    {
        serializeEntity(file, entityStore->entities + entityI);
    }
    
    SavFilePrintFormat(file, "end entity_store\n");
}

internal_func void serializeLevel(SavFile file, Level *level)
{
    Tilemap *tilemap = &level->levelTilemap;
    size_t count = (size_t)level->w * (size_t)level->h;

    SavFilePrintFormat(file, "begin level\n");
    SavFilePrintFormat(file, "world_x: %d\n", level->worldPos.x);
    SavFilePrintFormat(file, "world_y: %d\n", level->worldPos.y);

    {
        SavFilePrintFormat(file, "begin atlas_values\n");
        size_t encodedSize = 0;
        u8 *encodedData = SavBase64Encode((u8 *)tilemap->atlasValues, count * sizeof(*tilemap->atlasValues), &encodedSize);
        SavFileWriteBytes(file, encodedData, encodedSize);
        SavFree((void **)&encodedData);
        SavFilePrintFormat(file, "\nend atlas_values\n");
    }

    {
        SavFilePrintFormat(file, "begin colors\n");
        size_t encodedSize = 0;
        u8 *encodedData = SavBase64Encode((u8 *)tilemap->colors, count * sizeof(*tilemap->colors), &encodedSize);
        SavFileWriteBytes(file, encodedData, encodedSize);
        SavFree((void **)&encodedData);
        SavFilePrintFormat(file, "\nend colors\n");
    }

    {
        SavFilePrintFormat(file, "begin flags\n");
        size_t encodedSize = 0;
        u8 *encodedData = SavBase64Encode((u8 *)level->tileFlags, count * sizeof(*level->tileFlags), &encodedSize);
        SavFileWriteBytes(file, encodedData, encodedSize);
        SavFree((void **)&encodedData);
        SavFilePrintFormat(file, "\nend flags\n");
    }
        
    SavFilePrintFormat(file, "end level\n");
 }

internal_func void serializeLevelStore(SavFile file, LevelStore *levelStore)
{
    SavFilePrintFormat(file, "begin level_store\n");
    
    SavFilePrintFormat(file, "standard_level_width: %d\n", levelStore->standardLevelWidth);
    SavFilePrintFormat(file, "standard_level_height: %d\n", levelStore->standardLevelHeight);
    
    for (int levelI = 0; levelI < levelStore->levelCount; levelI++)
    {
        serializeLevel(file, levelStore->levels + levelI);
    }

    SavFilePrintFormat(file, "\nend level_store\n");
}

api_func void SaveGame(GameState *gameState)
{
    SavFile file = SavOpenFile("temp/saves/test_save.txt", SavFileOpenType_Write);

    serializeEntityStore(file, &gameState->entityStore);
    
    serializeLevelStore(file, &gameState->levelStore);

    // TODO
    // Camera
    // UIs
    // Game Log
    
    SavCloseFile(&file);
}

api_func void LoadGame()
{
}

