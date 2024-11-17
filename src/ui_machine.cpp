#include "ui_machine.h"

#include "sav.h"
#include "entity.h"
#include "entity_machine.h"
#include "inventory.h"

#include <imgui.h>

#include "ui_inventory.h"

api_func void DrawMachineUI(EntityStore *entityStore, InventoryStore *inventoryStore, Entity *e)
{
    Assert(e->type == EntityType_Machine);

    MakeStringBufferOnStack(windowTitle, 128);
    StringFormat("Machine -- %s (%d)###WINDOW_MACHINE_%d", windowTitle, e->name, e->id, e->id);
    ImGui::Begin(windowTitle.string);

    ImGui::BeginChild("InventoryChild",
                      ImVec2(0.0f, 0.0f),
                      ImGuiChildFlags_FrameStyle,
                      ImGuiWindowFlags_HorizontalScrollbar);

    ImVec2 renderedItemSize = ImVec2(32.0f, 32.0f);
    EntityDataMachine *m = &e->machineData;
    b32 anyItemsRendered = false;
    for (int i = 0; i < CAMPFIRE_ITEM_COUNT; i++)
    {
        int itemId = m->processedItemIds[i];
        if (itemId > 0)
        {
            
            InventoryItem *item = FindItemInInventoryById(e->inventory, itemId);
            if (item != NULL)
            {
                ImGui::PushID(itemId);
                Sprite itemSprite = item->spec->sprite;
                SavTextureAtlas atlas = GetAtlasForSprite(itemSprite);
                Rect sourceRect = GetAtlasSourceRect(atlas, itemSprite.atlasValue);
                FourV2 texCoords = GetTextureRectTexCoords(atlas.texture, sourceRect);

                ImVec2 uv0 = ImVec2(texCoords.e[0].x, texCoords.e[0].y); // UV coordinates for upper-left
                ImVec2 uv1 = ImVec2(texCoords.e[2].x, texCoords.e[2].y);
                ImVec4 bgCol = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
                ImVec4 tintCol = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                ImGui::ImageButton("", atlas.texture.id, renderedItemSize, uv0, uv1, bgCol, tintCol);
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
                {
                    ItemDragAndDropPayload payload;
                    payload.item = item;
                    payload.sourceEntity = e;
                    // Set payload to carry the index of our item (could be anything)
                    ImGui::SetDragDropPayload("INV_DRAG_ITEM", &payload, sizeof(ItemDragAndDropPayload));

                    ImGui::ImageButton("DRAG_AND_DROP_PREVIEW", atlas.texture.id, renderedItemSize, uv0, uv1, bgCol, tintCol);
            
                    ImGui::EndDragDropSource();
                }

                ImGui::SameLine();

                // ImGui::Text("%.3f", m->processedItemTimers[i]);

                ImGui::ProgressBar(m->processedItemTimers[i] / COOKING_DONE_TIME, ImVec2(0.0f, 0.0f));

                ImGui::PopID();
            

                anyItemsRendered = true;
            }
        }
    }

    if (!anyItemsRendered)
    {
        ImGui::Text("No items processing!");
    }
    
    ImGui::EndChild();

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("INV_DRAG_ITEM", ImGuiDragDropFlags_AcceptPeekOnly))
        {
            ItemDragAndDropPayload dragAndDropPayload = *(ItemDragAndDropPayload *)payload->Data;
            InventoryItem *item = dragAndDropPayload.item;
            Entity *sourceEntity = dragAndDropPayload.sourceEntity;

            if (sourceEntity != e &&
                IsControlledEntity(entityStore, sourceEntity) &&
                IsInRange(e->p, sourceEntity->p, entityStore->controlledEntity->stats.attackReach))
            {
                ImGui::AcceptDragDropPayload("INV_DRAG_ITEM");
                if (payload->IsDelivery())
                {
                    MoveItemFromEntityToEntity(sourceEntity, e, inventoryStore, item);
                }
            }
        }
        ImGui::EndDragDropTarget();
    }
    
    ImGui::End();
}
