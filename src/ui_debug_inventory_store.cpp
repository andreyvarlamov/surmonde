#include "ui_debug_inventory_store.h"
#include "sav.h"
#include "inventory.h"
#include <imgui.h>

api_func void DrawDebugInventoryStoreUI(InventoryStore *s)
{
    ImGui::Begin("Inventory Store Debug");

    ImGui::Text("%d blocks", s->blockCount);

    MakeStringBufferOnStack(headerTitle, 128);
    
    for (int i = 0; i < s->blockCount; i++)
    {
        InventoryBlock *b = s->blocks + i;
        if (b->entity != NULL)
        {
            StringFormat("Block %d -- %s (%lld)###BLOCK%d", headerTitle, i, b->entity->name, b->entity->id, i);
            if (ImGui::CollapsingHeader(headerTitle.string))
            {
                for (int itemI = 0; itemI < b->itemCount; itemI++)
                {
                    ImGui::Text(b->items[itemI].spec->name);
                }
            }
        }
    }
    
    ImGui::End();
}
