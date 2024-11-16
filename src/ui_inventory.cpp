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
    // ImGui::PushID(e);
    MakeStringBufferOnStack(windowTitle, 128);
    StringFormat("Inventory -- %s (%lld)###INVENTORY_%p", windowTitle, e->name, e->id, e);
    ImGui::Begin(windowTitle.string);

    ImGui::Text("Hello, inventory!");

    ImGuiStyle& imguiStyle = ImGui::GetStyle();

    ImVec2 size = ImVec2(64.0f, 64.0f);

    f32 dropAreaHeight = size.y + 2 * imguiStyle.FramePadding.y + 2 * imguiStyle.ItemSpacing.y;

    ImGui::BeginChild("InventoryChild",
                      ImVec2(0.0f, ImGui::GetContentRegionAvail().y - dropAreaHeight),
                      ImGuiChildFlags_None,
                      ImGuiWindowFlags_HorizontalScrollbar);
    
    int itemsPerLine = (int)((ImGui::GetContentRegionAvail().x +  + imguiStyle.ItemSpacing.x) /
                             (size.x + imguiStyle.ItemSpacing.x + 2 * imguiStyle.FramePadding.x));



    int itemsInLine = 0;
    InventoryItemIterator it = GetInventoryItemIterator(inventoryStore, e->inventory);
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
        ImGui::ImageButton("###BUTT", atlas.texture.id, size, uv0, uv1, bgCol, tintCol);
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
        {
            // Set payload to carry the index of our item (could be anything)
            ImGui::SetDragDropPayload("INV_DRAG_ITEM", &item, sizeof(InventoryItem *));
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
    }
    
    ImGui::EndChild();
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("INV_DRAG_ITEM"))
        {
            InventoryItem *item = *(InventoryItem **)payload->Data;
            TraceLog("Inv %s", item->spec->name);
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));
    ImGui::BeginChild("DropAreaChild",
                      ImVec2(0.0f, 0.0f),
                      ImGuiChildFlags_None,
                      ImGuiWindowFlags_None);
    ImGui::EndChild();
    ImGui::PopStyleColor();
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("INV_DRAG_ITEM"))
        {
            InventoryItem *item = *(InventoryItem **)payload->Data;
            DropItemFromEntity(entityStore, e, inventoryStore, item);
        }
        ImGui::EndDragDropTarget();
    }
    
    ImGui::End();
    // ImGui::PopID();
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
