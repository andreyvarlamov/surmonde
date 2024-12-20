#include "ui_debug_entity.h"
#include "sav.h"
#include "entity.h"
#include "inventory.h"
#include <imgui.h>

api_func void DrawDebugEntityUI(EntityStore *s, InventoryStore *inventoryStore, Entity *e)
{
    MakeStringBufferOnStack(windowTitle, 128);
    StringFormat("Entity -- %s -- (%d)###WINDOW_DEBUG_ENTITY_%d", windowTitle, e->name, e->id, e->id);
    ImGui::Begin(windowTitle.string);

    ImGui::Text("Entity %d (%p)", e->id, e);
    ImGui::Text("Name: %s", e->name);
    ImGui::Text("");
    ImGui::Checkbox("Is Paused", (bool *)&e->isPaused);
    ImGui::Text("Type: %s", EntityTypeString[e->type]);
    ImGui::Text("Is Blocking: %s", e->isBlocking ? "true" : "false");
    ImGui::Text("Is Opaque: %s", e->isOpaque ? "true" : "false");
    ImGui::Text("");
    ImGui::Text("Health: %.0f/%.0f", e->stats.health, e->stats.maxHealth);
    ImGui::Text("Position: %.2f, %.2f", e->p.x, e->p.y);

    if (e->type == EntityType_Actor && ImGui::CollapsingHeader("AI"))
    {
        ImGui::Text("Is Controlled: %s", IsControlledEntity(s, e) ? "true" : "false");
        ImGui::Text("AI State: %s", ActorAiStateTypeString[e->aiState.type]);
        ImGui::Text("Current Order: %s", ActorOrderTypeString[e->currentOrder.type]);

        if (e->currentOrder.type != ACTOR_ORDER_NONE)
        {
            ImGui::Text("Order Completed: %s", e->currentOrder.isCompleted ? "true" : "false");
        }

        switch (e->currentOrder.type)
        {
            case ACTOR_ORDER_MOVE_TO_TARGET:
            {
                ImGui::Text("Target: %.2f, %.2f", e->currentOrder.movementTarget.x, e->currentOrder.movementTarget.y);
            } break;
        
            case ACTOR_ORDER_FOLLOW_ENTITY:
            {
                ImGui::Text("Followed Entity: %s (%d)", e->currentOrder.entityToFollow->name, e->currentOrder.entityToFollow->id);
                ImGui::Text("Followed Entity Position: %.2f, %.2f", e->currentOrder.entityToFollow->p.x, e->currentOrder.entityToFollow->p.y);
            } break;

            case ACTOR_ORDER_ATTACK_ENTITY:
            { 
                ImGui::Text("Entity to Attack: %s (%d)", e->currentOrder.entityToAttack->name, e->currentOrder.entityToAttack->id);
                ImGui::Text("Attack Timer: %.2f", e->currentOrder.attackTimer);
            } break;

            default: break;
        }
    }

    if (ImGui::CollapsingHeader("Inventory"))
    {
        ImGui::Text("Has Inventory Block: %s", e->inventory != NULL ? "true" : "false");

        static InventoryItemSpecType selectedItem = (InventoryItemSpecType)(ItemType_None + 1);
        if (ImGui::BeginCombo("###ITEM_SELECTOR", GetInventoryItemSpecByType(selectedItem)->name))
        {
            for (int i = ItemType_None + 1; i < ItemType_Count; i++)
            {
                bool isSelected = (selectedItem == (InventoryItemSpecType)i);
                if (ImGui::Selectable(GetInventoryItemSpecByType((InventoryItemSpecType)i)->name, isSelected))
                {
                    selectedItem = (InventoryItemSpecType)i;
                }

                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine();
        if (ImGui::Button("Add Item"))
        {
            AddItemToEntityInventory(e, inventoryStore, InstantiateInventoryItemFromSpec(selectedItem, &inventoryStore->itemIdSeed));
        }
        
        InventoryItemIterator iterator = GetInventoryItemIterator(inventoryStore, e->inventory);
        for (InventoryItem *item = NextInventoryItem(&iterator);
             item != NULL;
             item = NextInventoryItem(&iterator))
        {
            ImGui::Text(item->spec->name);
            ImGui::SameLine();
            ImGui::PushID(item);
            if (ImGui::Button("Remove"))
            {
                RemoveItemFromEntityInventory(e, inventoryStore, item);
                goto stop_iterating;
            }
            if (e->type == EntityType_ItemPickup && IsInRange(e->p, s->controlledEntity->p, s->controlledEntity->stats.attackReach))
            {
                ImGui::SameLine();
                if (ImGui::Button("Pick up"))
                {
                    MoveItemFromEntityToEntity(e, s->controlledEntity, inventoryStore, item);
                    goto stop_iterating;
                }
            }
            if (e->type == EntityType_Actor)
            {
                ImGui::SameLine();
                if (ImGui::Button("Drop"))
                {
                    DropItemFromEntity(s, e, inventoryStore, item);
                    goto stop_iterating;
                }
            }

            ImGui::PopID();
            continue;

        stop_iterating:
            ImGui::PopID();
            break;
        }
    }
    
    ImGui::End();
}
