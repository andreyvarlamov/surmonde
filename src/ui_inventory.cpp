#include "ui_inventory.h"

#include "sav.h"
#include "entity.h"
#include "inventory.h"

#include <imgui.h>

#undef Min
#undef Max

#include <imgui_internal.h>

api_func void DrawInventoryUI(InventoryStore *inventoryStore, Entity *e)
{
    ImGui::Begin("Inventory");

    ImGui::Text("Hello, inventory!");

    ImGuiStyle& imguiStyle = ImGui::GetStyle();

    ImVec2 size = ImVec2(64.0f, 64.0f);
    
    int itemsPerLine = (int)((ImGui::GetContentRegionAvail().x - 2*imguiStyle.ItemSpacing.x) / (size.x + imguiStyle.ItemSpacing.x));

    #if 0
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec4 colF = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
    ImU32 col = ImColor(colF);
    ImVec2 rect0 = ImGui::GetCursorScreenPos();
    ImVec2 rect1 = ImVec2(rect0.x + ImGui::GetContentRegionAvail().x, rect0.y + ImGui::GetContentRegionAvail().y);
    drawList->AddRect(rect0, rect1, col, 0.0f, ImDrawFlags_None, 1.0f);
    #endif

    int itemsInLine = 0;
    InventoryItemIterator it = GetInventoryItemIterator(inventoryStore, e->inventory);
    for (InventoryItem *item = NextInventoryItem(&it);
         item != NULL;
         item = NextInventoryItem(&it))
    {
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
            // ImGui::SetDragDropPayload("DND_DEMO_CELL", &n, sizeof(int));

            // Display preview (could be anything, e.g. when dragging an image we could decide to display
            // the filename and a small preview of the image, etc.)
            // if (mode == Mode_Copy) { ImGui::Text("Copy %s", names[n]); }
            // if (mode == Mode_Move) { ImGui::Text("Move %s", names[n]); }
            // if (mode == Mode_Swap) { ImGui::Text("Swap %s", names[n]); }
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
    }
    
    ImGui::End();

    // ImRect rect(ImVec2(0.0f, 0.0f), ImVec2(500.0f, 500.0f));
    ImRect rect(ImGui::GetWindowPos(), ImGui::GetWindowSize());
    if (ImGui::BeginDragDropTargetCustom(rect, ImGui::GetID("panel")))
    {
        TraceLog("Dropped");
        ImGui::EndDragDropTarget();
    }
}
