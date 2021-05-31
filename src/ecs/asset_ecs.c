#define EV_ECS_TYPES_INCLUDE
#include <asset_ecs.h>

#include <flecs.h>
#include <evol/common/ev_types.h>

typedef struct {
  evstring name;
  U32 size;
  U32 alignment;
} AssetComponent;

typedef struct {
  evstring name;
} AssetTag;

typedef struct {
  evstring name;
  evstring sig;
  ECSPipelineStage stage;
  FN_PTR fn;
  F32 rate;
} AssetSystem;

typedef struct {
  evstring trigger_name;
  evstring cmp_name;
  void(*fn)(ECSQuery);
  enum {
    OnAddTrigger,
    OnRemoveTrigger,
    OnSetTrigger
  } type;

} AssetTrigger;

typedef struct {
  ecs_world_t *ecs_world;
  vec(ECSComponentID) components;
  vec(ECSTagID) tags;
  vec(ECSSystemID) systems;

  ecs_entity_t pipeline_stages[EV_ECS_PIPELINE_STAGE_COUNT];
} AssetWorld;

struct {
  vec(AssetWorld) assetWorlds;
  vec(AssetComponent) assetComponents;
  vec(AssetTag) assetTags;
  vec(AssetSystem) assetSystems;
  vec(AssetTrigger) assetTriggers;

  ECSAssetWorldHandle activeWorld;
} AssetECSData;

#define ActiveAssetWorld AssetECSData.assetWorlds[AssetECSData.activeWorld]

void
assetworld_destr(
    void *data)
{
  AssetWorld *world = (AssetWorld *)data;

  if(world->ecs_world) {
    ecs_fini(world->ecs_world);
  }
  if(world->components) {
    vec_fini(world->components);
  }
  if(world->tags) {
    vec_fini(world->tags);
  }
  if(world->systems) {
    vec_fini(world->systems);
  }
}

void
assetcomponent_destr(
    void *data)
{
  AssetComponent *comp = (AssetComponent *)data;
  evstring_free(comp->name);
}

void
assetcomponent_copy(
    void *dst, 
    const void *src)
{
  AssetComponent *dstComp = (AssetComponent *)dst;
  const AssetComponent *srcComp = (AssetComponent *)src;

  dstComp->alignment = srcComp->alignment;
  dstComp->size = srcComp->size;
  dstComp->name = evstring_new(srcComp->name);
}

void
assetsystem_copy(
    void *dst,
    const void *src)
{
  AssetSystem *dstSys = (AssetSystem *)dst;
  AssetSystem *srcSys = (AssetSystem *)src;

  dstSys->name  = evstring_new(srcSys->name);
  dstSys->sig   = evstring_new(srcSys->sig);
  dstSys->fn    = srcSys->fn;
  dstSys->stage = srcSys->stage;
  dstSys->rate  = srcSys->rate;
}

void
assetsystem_destr(
    void *data)
{
  AssetSystem *system = (AssetSystem*)data;
  evstring_free(system->name);
  evstring_free(system->sig);
}

void
assettag_destr(
    void *data)
{
  AssetTag *tag = (AssetTag *)data;
  evstring_free(tag->name);
}

void
assettag_copy(
    void *dst,
    const void *src)
{
  AssetTag *dstTag = (AssetTag*)dst;
  const AssetTag *srcTag = (AssetTag*)src;

  dstTag->name = evstring_new(srcTag->name);
}

void
assettrigger_copy(
    void *dst,
    const void *src)
{
  AssetTrigger *dstTrigger = (AssetTrigger*)dst;
  const AssetTrigger *srcTrigger = (AssetTrigger*)src;

  dstTrigger->cmp_name = evstring_new(srcTrigger->cmp_name);
  dstTrigger->trigger_name = evstring_new(srcTrigger->trigger_name);
  dstTrigger->fn = srcTrigger->fn;
  dstTrigger->type = srcTrigger->type;
}

void
assettrigger_destr(
    void *data)
{
  AssetTrigger *trigger = (AssetTrigger*) data;
  evstring_free(trigger->cmp_name);
  evstring_free(trigger->trigger_name);
}

void
ev_assetecs_init()
{
  AssetECSData.assetWorlds = vec_init(AssetWorld, NULL, assetworld_destr);
  vec_setlen((vec_t*)&AssetECSData.assetWorlds, 1);
  AssetECSData.assetWorlds[0] = (AssetWorld){NULL};
  AssetECSData.activeWorld = 0;

  AssetECSData.assetComponents = vec_init(AssetComponent, assetcomponent_copy, assetcomponent_destr);
  AssetECSData.assetTags = vec_init(AssetTag, assettag_copy, assettag_destr);
  AssetECSData.assetSystems = vec_init(AssetSystem, assetsystem_copy, assetsystem_destr);
  AssetECSData.assetTriggers = vec_init(AssetTrigger, assettrigger_copy, assettrigger_destr);
}

void
ev_assetecs_deinit()
{
  vec_fini(AssetECSData.assetTriggers);
  vec_fini(AssetECSData.assetSystems);
  vec_fini(AssetECSData.assetTags);
  vec_fini(AssetECSData.assetComponents);
  vec_fini(AssetECSData.assetWorlds);
  AssetECSData.activeWorld = 0;
}

ECSAssetWorldHandle
ev_assetecs_newworld()
{
  AssetWorld new_assetworld = {
    .ecs_world = ecs_init(),
    .components = vec_init(ECSComponentID),
    .tags = vec_init(ECSTagID),
    .systems = vec_init(ECSSystemID)
  };

  new_assetworld.pipeline_stages[EV_ECS_PIPELINE_STAGE_PREUPDATE] = EcsPreUpdate;
  new_assetworld.pipeline_stages[EV_ECS_PIPELINE_STAGE_UPDATE] = EcsOnUpdate;
  new_assetworld.pipeline_stages[EV_ECS_PIPELINE_STAGE_POSTUPDATE] = EcsPostUpdate;

  ecs_enable_locking(new_assetworld.ecs_world, 1);

  vec_setlen((vec_t*)&new_assetworld.components, vec_len(AssetECSData.assetComponents));
  for(size_t i = 0; i < vec_len(AssetECSData.assetComponents); i++) {
    AssetComponent component = AssetECSData.assetComponents[i];
    new_assetworld.components[i] = ecs_new_component(new_assetworld.ecs_world, 0, component.name, component.size, component.alignment);
  }

  vec_setlen((vec_t*)&new_assetworld.tags, vec_len(AssetECSData.assetTags));
  for(size_t i = 0; i < vec_len(AssetECSData.assetTags); i++) {
    AssetTag tag = AssetECSData.assetTags[i];
    new_assetworld.tags[i] = ev_ecs_registertag(new_assetworld.ecs_world, tag.name);
  }

  vec_setlen((vec_t*)&new_assetworld.systems, vec_len(AssetECSData.assetSystems));
  for(size_t i = 0; i < vec_len(AssetECSData.assetSystems); i++) {
    AssetSystem sys = AssetECSData.assetSystems[i];
    new_assetworld.systems[i] = ev_ecs_registersystem(new_assetworld.ecs_world, sys.sig, new_assetworld.pipeline_stages[sys.stage], sys.fn, sys.name);
    if(sys.rate > 0.f) {
      ev_ecs_setsystemrate(new_assetworld.ecs_world, new_assetworld.systems[i], sys.rate);
    }
  }

  for(size_t i = 0; i < vec_len(AssetECSData.assetTriggers); i++) {
    AssetTrigger trigger = AssetECSData.assetTriggers[i];
    switch(trigger.type) {
      case OnAddTrigger:
        ev_ecs_setonaddtrigger(new_assetworld.ecs_world, trigger.trigger_name, trigger.cmp_name, trigger.fn);
        break;
      case OnRemoveTrigger:
        ev_ecs_setonremovetrigger(new_assetworld.ecs_world, trigger.trigger_name, trigger.cmp_name, trigger.fn);
        break;
      case OnSetTrigger:
        ev_ecs_setonsettrigger(new_assetworld.ecs_world, trigger.trigger_name, trigger.cmp_name, trigger.fn);
        break;
      default:
        assert(!"Unhandled trigger type");
    }
  }

  ECSAssetWorldHandle newworld_handle = vec_push((vec_t*)&AssetECSData.assetWorlds, &new_assetworld);
  if(AssetECSData.activeWorld == 0) {
    AssetECSData.activeWorld = newworld_handle;
  }

  return newworld_handle;
}

AssetComponentID 
ev_assetecs_registercomponent(
    CONST_STR cmp_name, 
    U32 cmp_size, 
    U32 cmp_align)
{
  AssetComponentID id = vec_push((vec_t*)&AssetECSData.assetComponents, &(AssetComponent) {
      .name = cmp_name,
      .size = cmp_size,
      .alignment = cmp_align
  });

  for(size_t worldIndex = 1; worldIndex < vec_len(AssetECSData.assetWorlds); worldIndex++) {
    if(AssetECSData.assetWorlds[worldIndex].ecs_world == NULL) continue;

    ECSComponentID comp_id = ecs_new_component(AssetECSData.assetWorlds[worldIndex].ecs_world, 0, cmp_name, cmp_size, cmp_align);
    vec_push((vec_t*)&AssetECSData.assetWorlds[worldIndex].components, &comp_id);
    DEBUG_ASSERT(vec_len(AssetECSData.assetComponents) == vec_len(AssetECSData.assetWorlds[worldIndex].components));
  }

  return id;
}

AssetTagID 
ev_assetecs_registertag(
    CONST_STR tag_name)
{
  AssetTagID id = vec_push((vec_t*)&AssetECSData.assetTags, &(AssetTag) {
      .name = tag_name,
  });

  for(size_t worldIndex = 1; worldIndex < vec_len(AssetECSData.assetWorlds); worldIndex++) {
    if(AssetECSData.assetWorlds[worldIndex].ecs_world == NULL) continue;

    ECSTagID tag_id = ev_ecs_registertag(AssetECSData.assetWorlds[worldIndex].ecs_world, tag_name);
    vec_push((vec_t*)&AssetECSData.assetWorlds[worldIndex].tags, &tag_id);
    DEBUG_ASSERT(vec_len(AssetECSData.assetTags) == vec_len(AssetECSData.assetWorlds[worldIndex].tags));
  }
  return id;
}

U32 
ev_assetecs_setcomponent(
    ECSAssetWorldHandle world,
    AssetEntityID entt, 
    AssetComponentID cmp,
    PTR data)
{
  ecs_set_ptr_w_entity(AssetECSData.assetWorlds[world].ecs_world, entt, AssetECSData.assetWorlds[world].components[cmp], AssetECSData.assetComponents[cmp].size, data);
  return 0;
}

PTR
ev_assetecs_getcomponent(
    ECSAssetWorldHandle world_handle,
    AssetEntityID entt,
    AssetComponentID cmp)
{
  AssetWorld world = AssetECSData.assetWorlds[world_handle?world_handle:AssetECSData.activeWorld];
  return ecs_get_mut_w_entity(world.ecs_world, entt, world.components[cmp], NULL);
}

bool
ev_assetecs_hascomponent(
    ECSAssetWorldHandle world_handle,
    AssetEntityID entt,
    AssetComponentID cmp)
{
  AssetWorld world = AssetECSData.assetWorlds[world_handle?world_handle:AssetECSData.activeWorld];
  return ecs_has_entity(world.ecs_world, entt, world.components[cmp]);
}

U32
ev_assetecs_addtag(
    ECSAssetWorldHandle world_handle,
    AssetEntityID entt,
    AssetTagID tag)
{
  AssetWorld world = AssetECSData.assetWorlds[world_handle?world_handle:AssetECSData.activeWorld];
  ecs_add_entity(world.ecs_world, entt, world.tags[tag]);

  return 0;
}

U32
ev_assetecs_addcomponent(
    ECSAssetWorldHandle world_handle,
    AssetEntityID entt,
    AssetComponentID cmp)
{
  AssetWorld world = AssetECSData.assetWorlds[world_handle?world_handle:AssetECSData.activeWorld];
  ecs_add_entity(world.ecs_world, entt, world.components[cmp]);
  return 0;
}

AssetSystemID
ev_assetecs_registersystem(
    CONST_STR sig, 
    ECSPipelineStage stage, 
    FN_PTR fn, 
    CONST_STR name)
{
  AssetSystemID id = vec_push((vec_t*)&AssetECSData.assetSystems, &(AssetSystem) {
      .name = name,
      .stage = stage,
      .fn = fn,
      .sig = sig
  });

  for(size_t worldIndex = 1; worldIndex < vec_len(AssetECSData.assetWorlds); worldIndex++) {
    if(AssetECSData.assetWorlds[worldIndex].ecs_world == NULL) continue;

    ECSSystemID sys_id = ev_ecs_registersystem(AssetECSData.assetWorlds[worldIndex].ecs_world, sig, AssetECSData.assetWorlds[worldIndex].pipeline_stages[stage], fn, name);
    vec_push((vec_t*)&AssetECSData.assetWorlds[worldIndex].systems, &sys_id);
    DEBUG_ASSERT(vec_len(AssetECSData.assetSystems) == vec_len(AssetECSData.assetWorlds[worldIndex].systems));
  }
  return id;
}

void
ev_assetecs_setsystemrate(
    AssetSystemID system,
    F32 rate)
{
  AssetECSData.assetSystems[system].rate = rate;

  for(size_t worldIndex = 1; worldIndex < vec_len(AssetECSData.assetWorlds); worldIndex++) {
    if(AssetECSData.assetWorlds[worldIndex].ecs_world == NULL) continue;

    ev_ecs_setsystemrate(
        AssetECSData.assetWorlds[worldIndex].ecs_world, 
        AssetECSData.assetWorlds[worldIndex].systems[system], 
        rate);
  }

}

void
ev_assetecs_setonaddtrigger(
    CONST_STR trigger_name,
    CONST_STR cmp_name,
    void(*onaddfn)(ECSQuery))
{  
  vec_push((vec_t*)&AssetECSData.assetTriggers, &(AssetTrigger) {
      .trigger_name = trigger_name,
      .cmp_name = cmp_name,
      .fn = onaddfn,
      .type = OnAddTrigger
  });

  for(size_t worldIndex = 1; worldIndex < vec_len(AssetECSData.assetWorlds); worldIndex++) {
    if(AssetECSData.assetWorlds[worldIndex].ecs_world == NULL) continue;

    ev_ecs_setonaddtrigger(AssetECSData.assetWorlds[worldIndex].ecs_world, trigger_name, cmp_name, onaddfn);
  }
}

void
ev_assetecs_setonremovetrigger(
    CONST_STR trigger_name,
    CONST_STR cmp_name,
    void(*onremovefn)(ECSQuery))
{  
  vec_push((vec_t*)&AssetECSData.assetTriggers, &(AssetTrigger) {
      .trigger_name = trigger_name,
      .cmp_name = cmp_name,
      .fn = onremovefn,
      .type = OnRemoveTrigger
  });

  for(size_t worldIndex = 1; worldIndex < vec_len(AssetECSData.assetWorlds); worldIndex++) {
    if(AssetECSData.assetWorlds[worldIndex].ecs_world == NULL) continue;

    ev_ecs_setonremovetrigger(AssetECSData.assetWorlds[worldIndex].ecs_world, trigger_name, cmp_name, onremovefn);
  }

}

void
ev_assetecs_setonsettrigger(
    CONST_STR trigger_name,
    CONST_STR cmp_name,
    void(*onsetfn)(ECSQuery))
{  
  vec_push((vec_t*)&AssetECSData.assetTriggers, &(AssetTrigger) {
      .trigger_name = trigger_name,
      .cmp_name = cmp_name,
      .fn = onsetfn,
      .type = OnSetTrigger
  });

  for(size_t worldIndex = 1; worldIndex < vec_len(AssetECSData.assetWorlds); worldIndex++) {
    if(AssetECSData.assetWorlds[worldIndex].ecs_world == NULL) continue;

    ev_ecs_setonsettrigger(AssetECSData.assetWorlds[worldIndex].ecs_world, trigger_name, cmp_name, onsetfn);
  }
}

bool
ev_assetecs_hastag(
    ECSAssetWorldHandle world_handle,
    AssetEntityID entt,
    AssetTagID tag)
{
  AssetWorld world = AssetECSData.assetWorlds[world_handle];
  return ecs_has_entity(world.ecs_world, entt, world.tags[tag]);
}

void
ev_assetecs_foreachchild(
    ECSAssetWorldHandle world_handle,
    AssetEntityID entt,
    void(*iter_fn)(ECSAssetWorldHandle, AssetEntityID))
{
  AssetWorld world = AssetECSData.assetWorlds[world_handle];

  ecs_iter_t it = ecs_scope_iter(world.ecs_world, entt);
  while(ecs_scope_next(&it)) {
    for (int i = 0; i < it.count; i ++) {
      iter_fn(world_handle, it.entities[i]);
    }
  }
}

AssetEntityID
ev_assetecs_getparent(
    ECSAssetWorldHandle world_handle,
    AssetEntityID entt)
{
  return ecs_get_parent_w_entity(AssetECSData.assetWorlds[world_handle].ecs_world, entt, 0);
}

void
ev_assetecs_removetag(
    ECSAssetWorldHandle world_handle,
    AssetEntityID entt,
    AssetTagID tag)
{
  AssetWorld world = AssetECSData.assetWorlds[world_handle];
  ecs_remove_entity(world.ecs_world, entt, world.tags[tag]);
}

void
ev_assetecs_setsingleton(
    ECSAssetWorldHandle world_handle,
    AssetComponentID comp,
    PTR data)
{
  AssetWorld world = AssetECSData.assetWorlds[world_handle];
  ECSComponentID cmp = world.components[comp];
  size_t cmp_size = AssetECSData.assetComponents[comp].size;
  ev_ecs_setcomponent(world.ecs_world, cmp, cmp, cmp_size, data);
  ev_ecs_modified(world.ecs_world, cmp, cmp);
}

PTR
ev_assetecs_getsingleton(
    ECSAssetWorldHandle world_handle,
    AssetComponentID comp)
{
  AssetWorld world = AssetECSData.assetWorlds[world_handle];
  ECSComponentID cmp = world.components[comp];
  return ev_ecs_getcomponent(world.ecs_world, cmp, cmp);
}

void
ev_assetecs_modified(
    ECSAssetWorldHandle world_handle,
    AssetEntityID entt,
    AssetComponentID comp)
{
  AssetWorld world = AssetECSData.assetWorlds[world_handle];
  ev_ecs_modified(world.ecs_world, entt, world.components[comp]);
}

AssetEntityID
ev_assetecs_createentity(
    ECSAssetWorldHandle world_handle)
{
  return ecs_new(AssetECSData.assetWorlds[world_handle].ecs_world, 0);
}


void
ev_assetecs_destroyentity(
    ECSAssetWorldHandle world_handle,
    AssetEntityID entt)
{
  AssetWorld world = AssetECSData.assetWorlds[world_handle];
  ecs_delete(world.ecs_world, entt);
}

AssetEntityID
ev_assetecs_createchildentity(
    ECSAssetWorldHandle world_handle,
    AssetEntityID parent)
{
  AssetWorld world = AssetECSData.assetWorlds[world_handle];
  return ecs_new_w_entity(world.ecs_world, ECS_CHILDOF | parent);
}

void
ev_assetecs_addchildtoentity(
    ECSAssetWorldHandle world_handle,
    AssetEntityID parent,
    AssetEntityID child)
{
  AssetWorld world = AssetECSData.assetWorlds[world_handle];
  ecs_add_entity(world.ecs_world, child, ECS_CHILDOF | parent);
}

void
ev_assetecs_destroyworld(
    ECSAssetWorldHandle world_handle)
{
  AssetWorld world = AssetECSData.assetWorlds[world_handle];
  ecs_fini(world.ecs_world);
  vec_fini(world.components);
  vec_fini(world.tags);
  vec_fini(world.systems);
}

U32
ev_assetecs_progress(
    ECSAssetWorldHandle world_handle,
    F32 deltaTime)
{
  AssetWorld world = AssetECSData.assetWorlds[world_handle];
  return ecs_progress(world.ecs_world, deltaTime)?0:1;
}
