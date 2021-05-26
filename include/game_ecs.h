#pragma once
#include <common_ecs.h>

void
ev_gameecs_init();

void
ev_gameecs_deinit();

ECSGameWorldHandle
ev_gameecs_newworld();

GameComponentID 
ev_gameecs_registercomponent(
  CONST_STR cmp_name, 
  U32 cmp_size, 
  U32 cmp_align);

U32
ev_gameecs_update(
  F32 deltaTime);

PTR
ev_gameecs_getcomponent(
    ECSGameWorldHandle world_handle,
    GameEntityID entt,
    GameComponentID cmp);

U32 
ev_gameecs_setcomponent(
    ECSGameWorldHandle world,
    GameEntityID entt, 
    GameComponentID cmp,
    PTR data);

bool
ev_gameecs_hascomponent(
    ECSGameWorldHandle world_handle,
    GameEntityID entt,
    GameComponentID cmp);

U32
ev_gameecs_addtag(
    ECSGameWorldHandle world_handle,
    GameEntityID entt,
    GameTagID tag);

U32
ev_gameecs_addcomponent(
    ECSGameWorldHandle world_handle,
    GameEntityID entt,
    GameComponentID cmp);

GameTagID 
ev_gameecs_registertag(
    CONST_STR tag_name);

GameSystemID
ev_gameecs_registersystem(
    CONST_STR sig, 
    ECSPipelineStage stage, 
    FN_PTR fn, 
    CONST_STR name);

void
ev_gameecs_setsystemrate(
    GameSystemID system,
    F32 rate);

void
ev_gameecs_setonaddtrigger(
    CONST_STR trigger_name,
    CONST_STR cmp_name,
    void(*onaddfn)(ECSQuery));

void
ev_gameecs_setonremovetrigger(
    CONST_STR trigger_name,
    CONST_STR cmp_name,
    void(*onremovefn)(ECSQuery));

void
ev_gameecs_setonsettrigger(
    CONST_STR trigger_name,
    CONST_STR cmp_name,
    void(*onsetfn)(ECSQuery));

bool
ev_gameecs_hastag(
    ECSGameWorldHandle world_handle,
    GameEntityID entt,
    GameTagID tag);

void
ev_gameecs_foreachchild(
    ECSGameWorldHandle world_handle,
    GameEntityID entt,
    void(*iter_fn)(ECSGameWorldHandle, GameEntityID));

GameEntityID
ev_gameecs_getparent(
    ECSGameWorldHandle world_handle,
    GameEntityID entt);

void
ev_gameecs_removetag(
    ECSGameWorldHandle world_handle,
    GameEntityID entt,
    GameTagID tag);

void
ev_gameecs_setsingleton(
    ECSGameWorldHandle world_handle,
    GameComponentID comp,
    PTR data);

PTR
ev_gameecs_getsingleton(
    ECSGameWorldHandle world_handle,
    GameComponentID comp);

void
ev_gameecs_modified(
    ECSGameWorldHandle world_handle,
    GameEntityID entt,
    GameComponentID comp);

GameEntityID
ev_gameecs_createentity(
    ECSGameWorldHandle world_handle);

void
ev_gameecs_destroyentity(
    ECSGameWorldHandle world_handle,
    GameEntityID entt);

GameEntityID
ev_gameecs_createchildentity(
    ECSGameWorldHandle world_handle,
    GameEntityID parent);

void
ev_gameecs_addchildtoentity(
    ECSGameWorldHandle world_handle,
    GameEntityID parent,
    GameEntityID child);

void
ev_gameecs_destroyworld(
    ECSGameWorldHandle world_handle);

U32
ev_gameecs_progress(
    ECSGameWorldHandle world_handle,
    F32 deltaTime);