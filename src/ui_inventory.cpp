#include "ui_inventory.h"

#include "sav.h"
#include "entity.h"
#include "inventory.h"

#include <imgui.h>

#undef Min
#undef Max

#include <imgui_internal.h>

api_func void DrawInventoryUI(EntityStore *entityStore, InventoryStore *inventoryStore, Entity *e)
{
    if (e->type == EntityType_None)
    {
        return;
    }
    MakeStringBufferOnStack(windowTitle, 128);
    StringFormat("Inventory -- %s (%d)###WINDOW_INVENTORY_%d", windowTitle, e->name, e->id, e->id);
    ImGui::Begin(windowTitle.string);

    ImGuiStyle& imguiStyle = ImGui::GetStyle();

    ImVec2 size = ImVec2(32.0f, 32.0f);

    b32 isControlledEntity = IsControlledEntity(entityStore, e);
    
    f32 dropAreaHeight = size.y + 2 * imguiStyle.FramePadding.y + 2 * imguiStyle.ItemSpacing.y;
    f32 inventoryHeight = isControlledEntity ? (ImGui::GetContentRegionAvail().y - dropAreaHeight) : 0.0f;

    ImGui::BeginChild("InventoryChild",
                      ImVec2(0.0f, inventoryHeight),
                      ImGuiChildFlags_FrameStyle,
                      ImGuiWindowFlags_HorizontalScrollbar);
    
    int itemsPerLine = (int)((ImGui::GetContentRegionAvail().x +  + imguiStyle.ItemSpacing.x) /
                             (size.x + imguiStyle.ItemSpacing.x + 2 * imguiStyle.FramePadding.x));

    int itemsInLine = 0;
    InventoryItemIterator it = GetInventoryItemIterator(inventoryStore, e->inventory);
    b32 anyItemsRendered = false;
    for (InventoryItem *item = NextInventoryItem(&it);
         item != NULL;
         item = NextInventoryItem(&it))
    {
        ImGui::PushID(item);
        Sprite itemSprite = item->spec->sprite;
        SavTextureAtlas atlas = GetAtlasForSprite(itemSprite);
        Rect sourceRect = GetAtlasSourceRect(atlas, itemSprite.atlasValue);
        FourV2 texCoords = GetTextureRectTexCoords(atlas.texture, sourceRect);

        ImVec2 uv0 = ImVec2(texCoords.e[0].x, texCoords.e[0].y); // UV coordinates for upper-left
        ImVec2 uv1 = ImVec2(texCoords.e[2].x, texCoords.e[2].y);
        ImVec4 bgCol = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        ImVec4 tintCol = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        ImGui::ImageButton("", atlas.texture.id, size, uv0, uv1, bgCol, tintCol);
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
        {
            ItemDragAndDropPayload payload;
            payload.item = item;
            payload.sourceEntity = e;
            // Set payload to carry the index of our item (could be anything)
            ImGui::SetDragDropPayload("INV_DRAG_ITEM", &payload, sizeof(ItemDragAndDropPayload));

            ImGui::ImageButton("DRAG_AND_DROP_PREVIEW", atlas.texture.id, size, uv0, uv1, bgCol, tintCol);
            
            ImGui::EndDragDropSource();
        }

        if ((itemsInLine++) + 1 < itemsPerLine)
        {
            ImGui::SameLine();
        }
        else
        {
            itemsInLine = 0;
        }
        ImGui::PopID();

        anyItemsRendered = true;
    }

    if (!anyItemsRendered)
    {
        ImGui::Text("No items!");
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
                (isControlledEntity ||
                 IsControlledEntity(entityStore, sourceEntity)) &&
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

    if (isControlledEntity)
    {
        ImGui::BeginChild("DropAreaChild",
                          ImVec2(0.0f, 0.0f),
                          ImGuiChildFlags_Borders,
                          ImGuiWindowFlags_None);
        ImGui::EndChild();
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("INV_DRAG_ITEM", ImGuiDragDropFlags_AcceptPeekOnly))
            {
                ItemDragAndDropPayload dragAndDropPayload = *(ItemDragAndDropPayload *)payload->Data;
                InventoryItem *item = dragAndDropPayload.item;
                Entity *sourceEntity = dragAndDropPayload.sourceEntity;

                if (sourceEntity == e)
                {
                    ImGui::AcceptDragDropPayload("INV_DRAG_ITEM");
                    if (payload->IsDelivery())
                    {
                        DropItemFromEntity(entityStore, e, inventoryStore, item);
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }
    }
    
    ImGui::End();
}

/*
    local_persist f32 ratio = 0.0f;
    
    ImGui::SliderFloat("Ratio", &ratio, 0.0f, 1.0f, "ratio = %.3f");

    #if 1
    {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec4 colF = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
        ImU32 col = ImColor(colF);
        ImVec2 rect0 = ImGui::GetCursorScreenPos();
        ImVec2 rect1 = ImVec2(rect0.x + ImGui::GetContentRegionAvail().x, rect0.y + ImGui::GetContentRegionAvail().y);
        // ImVec2 rect1 = ImVec2(rect0.x + size.x, rect0.y + size.y);
        drawList->AddRect(rect0, rect1, col, 0.0f, ImDrawFlags_None, 1.0f);
    }
    #endif
 */
