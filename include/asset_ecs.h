#pragma once
#include <common_ecs.h>

void
ev_assetecs_init();

void
ev_assetecs_deinit();

ECSAssetWorldHandle
ev_assetecs_newworld();

AssetComponentID 
ev_assetecs_registercomponent(
  CONST_STR cmp_name, 
  U32 cmp_size, 
  U32 cmp_align);

U32
ev_assetecs_update(
  F32 deltaTime);

PTR
ev_assetecs_getcomponent(
    ECSAssetWorldHandle world_handle,
    AssetEntityID entt,
    AssetComponentID cmp);

U32 
ev_assetecs_setcomponent(
    ECSAssetWorldHandle world,
    AssetEntityID entt, 
    AssetComponentID cmp,
    PTR data);

bool
ev_assetecs_hascomponent(
    ECSAssetWorldHandle world_handle,
    AssetEntityID entt,
    AssetComponentID cmp);

U32
ev_assetecs_addtag(
    ECSAssetWorldHandle world_handle,
    AssetEntityID entt,
    AssetTagID tag);

U32
ev_assetecs_addcomponent(
    ECSAssetWorldHandle world_handle,
    AssetEntityID entt,
    AssetComponentID cmp);

AssetTagID 
ev_assetecs_registertag(
    CONST_STR tag_name);

AssetSystemID
ev_assetecs_registersystem(
    CONST_STR sig, 
    ECSPipelineStage stage, 
    FN_PTR fn, 
    CONST_STR name);

void
ev_assetecs_setsystemrate(
    AssetSystemID system,
    F32 rate);

void
ev_assetecs_setonaddtrigger(
    CONST_STR trigger_name,
    CONST_STR cmp_name,
    void(*onaddfn)(ECSQuery));

void
ev_assetecs_setonremovetrigger(
    CONST_STR trigger_name,
    CONST_STR cmp_name,
    void(*onremovefn)(ECSQuery));

void
ev_assetecs_setonsettrigger(
    CONST_STR trigger_name,
    CONST_STR cmp_name,
    void(*onsetfn)(ECSQuery));

bool
ev_assetecs_hastag(
    ECSAssetWorldHandle world_handle,
    AssetEntityID entt,
    AssetTagID tag);

void
ev_assetecs_foreachchild(
    ECSAssetWorldHandle world_handle,
    AssetEntityID entt,
    void(*iter_fn)(ECSAssetWorldHandle, AssetEntityID));

AssetEntityID
ev_assetecs_getparent(
    ECSAssetWorldHandle world_handle,
    AssetEntityID entt);

void
ev_assetecs_removetag(
    ECSAssetWorldHandle world_handle,
    AssetEntityID entt,
    AssetTagID tag);

void
ev_assetecs_setsingleton(
    ECSAssetWorldHandle world_handle,
    AssetComponentID comp,
    PTR data);

PTR
ev_assetecs_getsingleton(
    ECSAssetWorldHandle world_handle,
    AssetComponentID comp);

void
ev_assetecs_modified(
    ECSAssetWorldHandle world_handle,
    AssetEntityID entt,
    AssetComponentID comp);

AssetEntityID
ev_assetecs_createentity(
    ECSAssetWorldHandle world_handle);

void
ev_assetecs_destroyentity(
    ECSAssetWorldHandle world_handle,
    AssetEntityID entt);

AssetEntityID
ev_assetecs_createchildentity(
    ECSAssetWorldHandle world_handle,
    AssetEntityID parent);

void
ev_assetecs_addchildtoentity(
    ECSAssetWorldHandle world_handle,
    AssetEntityID parent,
    AssetEntityID child);

void
ev_assetecs_destroyworld(
    ECSAssetWorldHandle world_handle);

U32
ev_assetecs_progress(
    ECSAssetWorldHandle world_handle,
    F32 deltaTime);
