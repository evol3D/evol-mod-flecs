#pragma once

#include <flecs.h>
#if defined(EV_ECS_TYPES_INCLUDE)
# include <evol/common/ev_types.h>
# define TYPE(name, ...) typedef __VA_ARGS__ name;
# include <meta/evmod.types>
#endif

ECSTagID
ev_ecs_registertag(
    ecs_world_t *world,
    CONST_STR tag_name);

U32
ev_ecs_setcomponent(
    ecs_world_t *world,
    ECSEntityID entt, 
    ECSComponentID cmp,
    U32 cmp_size,
    PTR data);

U32
ev_ecs_addcomponent(
    ecs_world_t *world,
    ECSEntityID entt, 
    ECSComponentID cmp);

U32
ev_ecs_addtag(
    ecs_world_t *world,
    ECSEntityID entt,
    ECSTagID tag);

bool
ev_ecs_hastag(
    ecs_world_t *world,
    ECSEntityID entt,
    ECSTagID tag);

U32
ev_ecs_enableentity(
    ecs_world_t *world,
    ECSEntityID entt);

U32
ev_ecs_disableentity(
    ecs_world_t *world,
    ECSEntityID entt);

ECSEntityID
ev_ecs_getparent(
    ecs_world_t *world,
    ECSEntityID entt);

ECSSystemID
ev_ecs_registersystem(
    ecs_world_t *world,
    CONST_STR sig, 
    ecs_entity_t stage, 
    FN_PTR fn, 
    CONST_STR name);

void
ev_ecs_setsystemrate(
    ecs_world_t *world,
    ECSSystemID system,
    F32 rate);

PTR
ev_ecs_getquerycolumn(
    ECSQuery query, 
    U32 size, 
    U32 idx);

U32
ev_ecs_getquerymatchcount(
    ECSQuery query);

ECSEntityID *
ev_ecs_getqueryentities(
    ECSQuery query);

void
ev_ecs_modified(
    ecs_world_t *world,
    ECSEntityID entt,
    ECSComponentID comp);

// Assumes that the component is always mutable. Easier for scripting?
PTR
ev_ecs_getcomponent(
    ecs_world_t *world,
    ECSEntityID entt,
    ECSComponentID cmp);

bool
ev_ecs_hascomponent(
    ecs_world_t *world,
    ECSEntityID entt,
    ECSComponentID cmp);

void
ev_ecs_setsingleton(
    ecs_world_t *world,
    ECSComponentID cmp,
    U32 cmp_size,
    PTR data);

PTR
ev_ecs_getsingleton(
    ecs_world_t *world,
    ECSComponentID cmp);

void
ev_ecs_foreachchild(
    ecs_world_t *world,
    ECSEntityID entt,
    void(*iter_fn)(ECSEntityID));

void
ev_ecs_removetag(
    ecs_world_t *world,
    ECSEntityID entt,
    ECSTagID tag);

void
ev_ecs_setonaddtrigger(
    ecs_world_t *world,
    CONST_STR trigger_name,
    CONST_STR cmp_name,
    void(*onaddfn)(ECSQuery));

void
ev_ecs_setonremovetrigger(
    ecs_world_t *world,
    CONST_STR trigger_name,
    CONST_STR cmp_name,
    void(*onremovefn)(ECSQuery));

void
ev_ecs_setonsettrigger(
    ecs_world_t *world,
    CONST_STR trigger_name,
    CONST_STR cmp_name,
    void(*onsetfn)(ECSQuery));

U32 
ev_ecs_unlock(
    ecs_world_t *world);

U32 
ev_ecs_lock(
    ecs_world_t *world);

ECSComponentID 
ev_ecs_registercomponent(
    ecs_world_t *world,
    CONST_STR cmp_name, 
    U32 cmp_size, 
    U32 cmp_align);

ECSEntityID 
ev_ecs_createentity(
    ecs_world_t *world);

void
ev_ecs_deleteentity(
    ecs_world_t *world,
    ECSEntityID entt);

void
ev_ecs_addchild(
    ecs_world_t *world,
    ECSEntityID parent,
    ECSEntityID child);

ECSEntityID
ev_ecs_createchild(
    ecs_world_t *world,
    ECSEntityID parent);
