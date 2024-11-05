#include "ui_debug_actor.h"
#include "sav.h"
#include "entity.h"
#include "imgui.h"

api_func void DrawDebugActorUI(EntityStore *s, Entity *e)
{
    MakeStringBufferOnStack(windowTitle, 128);
    StringFormat("Entity -- %s -- %lld", windowTitle, e->name, e->id);
    ImGui::Begin(windowTitle.string);

    ImGui::Text("Entity %lld (%p)", e->id, e);
    ImGui::Checkbox("Is Paused", (bool *)&e->isPaused);
    ImGui::Text("Name: %s", e->name);
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
            ImGui::Text("Followed Entity: %s (%lld)", e->currentOrder.followedEntity->name, e->currentOrder.followedEntity->id);
            ImGui::Text("Followed Entity Position: %.2f, %.2f", e->currentOrder.followedEntity->p.x, e->currentOrder.followedEntity->p.y);
        } break;

        case ACTOR_ORDER_ATTACK_ENTITY:
        {
            ImGui::Text("Entity to Attack: %s (%lld)", e->currentOrder.entityToAttack->name, e->currentOrder.entityToAttack->id);
            ImGui::Text("Attack Timer: %.2f", e->currentOrder.attackTimer);
        } break;

        default: break;
    }
    
    ImGui::End();
}
