#include "ui_inventory.h"

#include "sav.h"
#include "entity.h"
#include "inventory.h"

api_func void DrawInventoryUI(InventoryStore *inventoryStore, Entity *e)
{
    ImGui::Begin("Inventory");

    ImGui::Text("Hello, inventory!");

    InventoryItemIterator it = GetInventoryItemIterator(inventoryStore, e->inventory);
    for (InventoryItem *item = NextInventoryItem(&it);
         item != NULL;
         item = NextInventoryItem(&it))
    {
        Sprite itemSprite = item->spec->sprite;
        SavTextureAtlas atlas = GetAtlasForSprite(itemSprite);
        Rect sourceRect = GetAtlasSourceRect(atlas, itemSprite.atlasValue);
        FourV2 texCoords = GetTextureRectTexCoords(atlas.texture, sourceRect);

        ImVec2 size = ImVec2(32.0f, 32.0f);
        ImVec2 uv0 = ImVec2(texCoords.e[0].x, texCoords.e[0].y); // UV coordinates for upper-left
        ImVec2 uv1 = ImVec2(texCoords.e[2].x, texCoords.e[2].y);
        ImVec4 bgCol = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        ImVec4 tintCol = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        if (ImGui::ImageButton("###BUTT", atlas.texture.id, size, uv0, uv1, bgCol, tintCol))
        {
        }
    }
    
    ImGui::End();
}
