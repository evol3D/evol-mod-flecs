#define EV_ECS_TYPES_INCLUDE
#include <game_ecs.h>

#include <flecs.h>
#include <evol/common/ev_types.h>

typedef struct {
  evstring name;
  U32 size;
  U32 alignment;
} GameComponent;

typedef struct {
  evstring name;
} GameTag;

typedef struct {
  evstring name;
  evstring sig;
  ECSPipelineStage stage;
  FN_PTR fn;
  F32 rate;
} GameSystem;

typedef struct {
  evstring trigger_name;
  evstring cmp_name;
  void(*fn)(ECSQuery);
  enum {
    OnAddTrigger,
    OnRemoveTrigger,
    OnSetTrigger
  } type;

} GameTrigger;

typedef struct {
  ecs_world_t *ecs_world;
  vec(ECSComponentID) components;
  vec(ECSTagID) tags;
  vec(ECSSystemID) systems;

  ecs_entity_t pipeline_stages[EV_ECS_PIPELINE_STAGE_COUNT];
} GameWorld;

struct {
  vec(GameWorld) gameWorlds;
  vec(GameComponent) gameComponents;
  vec(GameTag) gameTags;
  vec(GameSystem) gameSystems;
  vec(GameTrigger) gameTriggers;

  ECSGameWorldHandle activeWorld;
} GameECSData;

#define ActiveGameWorld GameECSData.gameWorlds[GameECSData.activeWorld]

void
gameworld_destr(
    void *data)
{
  GameWorld *world = (GameWorld *)data;

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
gamecomponent_destr(
    void *data)
{
  GameComponent *comp = (GameComponent *)data;
  evstring_free(comp->name);
}

void
gamecomponent_copy(
    void *dst, 
    const void *src)
{
  GameComponent *dstComp = (GameComponent *)dst;
  const GameComponent *srcComp = (GameComponent *)src;

  dstComp->alignment = srcComp->alignment;
  dstComp->size = srcComp->size;
  dstComp->name = evstring_new(srcComp->name);
}

void
gamesystem_copy(
    void *dst,
    const void *src)
{
  GameSystem *dstSys = (GameSystem *)dst;
  GameSystem *srcSys = (GameSystem *)src;

  dstSys->name  = evstring_new(srcSys->name);
  dstSys->sig   = evstring_new(srcSys->sig);
  dstSys->fn    = srcSys->fn;
  dstSys->stage = srcSys->stage;
  dstSys->rate  = srcSys->rate;
}

void
gamesystem_destr(
    void *data)
{
  GameSystem *system = (GameSystem*)data;
  evstring_free(system->name);
  evstring_free(system->sig);
}

void
gametag_destr(
    void *data)
{
  GameTag *tag = (GameTag *)data;
  evstring_free(tag->name);
}

void
gametag_copy(
    void *dst,
    const void *src)
{
  GameTag *dstTag = (GameTag*)dst;
  const GameTag *srcTag = (GameTag*)src;

  dstTag->name = evstring_new(srcTag->name);
}

void
gametrigger_copy(
    void *dst,
    const void *src)
{
  GameTrigger *dstTrigger = (GameTrigger*)dst;
  const GameTrigger *srcTrigger = (GameTrigger*)src;

  dstTrigger->cmp_name = evstring_new(srcTrigger->cmp_name);
  dstTrigger->trigger_name = evstring_new(srcTrigger->trigger_name);
  dstTrigger->fn = srcTrigger->fn;
  dstTrigger->type = srcTrigger->type;
}

void
gametrigger_destr(
    void *data)
{
  GameTrigger *trigger = (GameTrigger*) data;
  evstring_free(trigger->cmp_name);
  evstring_free(trigger->trigger_name);
}

void
ev_gameecs_init()
{
  GameECSData.gameWorlds = vec_init(GameWorld, NULL, gameworld_destr);
  vec_setlen((vec_t*)&GameECSData.gameWorlds, 1);
  GameECSData.gameWorlds[0] = (GameWorld){NULL};
  GameECSData.activeWorld = 0;

  GameECSData.gameComponents = vec_init(GameComponent, gamecomponent_copy, gamecomponent_destr);
  GameECSData.gameTags = vec_init(GameTag, gametag_copy, gametag_destr);
  GameECSData.gameSystems = vec_init(GameSystem, gamesystem_copy, gamesystem_destr);
  GameECSData.gameTriggers = vec_init(GameTrigger, gametrigger_copy, gametrigger_destr);
}

void
ev_gameecs_deinit()
{
  vec_fini(GameECSData.gameTriggers);
  vec_fini(GameECSData.gameSystems);
  vec_fini(GameECSData.gameTags);
  vec_fini(GameECSData.gameComponents);
  vec_fini(GameECSData.gameWorlds);
  GameECSData.activeWorld = 0;
}

ECSGameWorldHandle
ev_gameecs_newworld()
{
  GameWorld new_gameworld = {
    .ecs_world = ecs_init(),
    .components = vec_init(ECSComponentID),
    .tags = vec_init(ECSTagID),
    .systems = vec_init(ECSSystemID)
  };

  new_gameworld.pipeline_stages[EV_ECS_PIPELINE_STAGE_PREUPDATE] = EcsPreUpdate;
  new_gameworld.pipeline_stages[EV_ECS_PIPELINE_STAGE_UPDATE] = EcsOnUpdate;
  new_gameworld.pipeline_stages[EV_ECS_PIPELINE_STAGE_POSTUPDATE] = EcsPostUpdate;

  ecs_enable_locking(new_gameworld.ecs_world, 1);

  vec_setlen((vec_t*)&new_gameworld.components, vec_len(GameECSData.gameComponents));
  for(size_t i = 0; i < vec_len(GameECSData.gameComponents); i++) {
    GameComponent component = GameECSData.gameComponents[i];
    new_gameworld.components[i] = ecs_new_component(new_gameworld.ecs_world, 0, component.name, component.size, component.alignment);
  }

  vec_setlen((vec_t*)&new_gameworld.tags, vec_len(GameECSData.gameTags));
  for(size_t i = 0; i < vec_len(GameECSData.gameTags); i++) {
    GameTag tag = GameECSData.gameTags[i];
    new_gameworld.tags[i] = ev_ecs_registertag(new_gameworld.ecs_world, tag.name);
  }

  vec_setlen((vec_t*)&new_gameworld.systems, vec_len(GameECSData.gameSystems));
  for(size_t i = 0; i < vec_len(GameECSData.gameSystems); i++) {
    GameSystem sys = GameECSData.gameSystems[i];
    new_gameworld.systems[i] = ev_ecs_registersystem(new_gameworld.ecs_world, sys.sig, new_gameworld.pipeline_stages[sys.stage], sys.fn, sys.name);
    if(sys.rate > 0.f) {
      ev_ecs_setsystemrate(new_gameworld.ecs_world, new_gameworld.systems[i], sys.rate);
    }
  }

  for(size_t i = 0; i < vec_len(GameECSData.gameTriggers); i++) {
    GameTrigger trigger = GameECSData.gameTriggers[i];
    switch(trigger.type) {
      case OnAddTrigger:
        ev_ecs_setonaddtrigger(new_gameworld.ecs_world, trigger.trigger_name, trigger.cmp_name, trigger.fn);
        break;
      case OnRemoveTrigger:
        ev_ecs_setonremovetrigger(new_gameworld.ecs_world, trigger.trigger_name, trigger.cmp_name, trigger.fn);
        break;
      case OnSetTrigger:
        ev_ecs_setonsettrigger(new_gameworld.ecs_world, trigger.trigger_name, trigger.cmp_name, trigger.fn);
        break;
      default:
        assert(!"Unhandled trigger type");
    }
  }

  ECSGameWorldHandle newworld_handle = vec_push((vec_t*)&GameECSData.gameWorlds, &new_gameworld);
  if(GameECSData.activeWorld == 0) {
    GameECSData.activeWorld = newworld_handle;
  }

  return newworld_handle;
}

GameComponentID 
ev_gameecs_registercomponent(
    CONST_STR cmp_name, 
    U32 cmp_size, 
    U32 cmp_align)
{
  GameComponentID id = vec_push((vec_t*)&GameECSData.gameComponents, &(GameComponent) {
      .name = cmp_name,
      .size = cmp_size,
      .alignment = cmp_align
  });

  for(size_t worldIndex = 1; worldIndex < vec_len(GameECSData.gameWorlds); worldIndex++) {
    if(GameECSData.gameWorlds[worldIndex].ecs_world == NULL) continue;

    ECSComponentID comp_id = ecs_new_component(GameECSData.gameWorlds[worldIndex].ecs_world, 0, cmp_name, cmp_size, cmp_align);
    vec_push((vec_t*)&GameECSData.gameWorlds[worldIndex].components, &comp_id);
    DEBUG_ASSERT(vec_len(GameECSData.gameComponents) == vec_len(GameECSData.gameWorlds[worldIndex].components));
  }

  return id;
}

GameTagID 
ev_gameecs_registertag(
    CONST_STR tag_name)
{
  GameTagID id = vec_push((vec_t*)&GameECSData.gameTags, &(GameTag) {
      .name = tag_name,
  });

  for(size_t worldIndex = 1; worldIndex < vec_len(GameECSData.gameWorlds); worldIndex++) {
    if(GameECSData.gameWorlds[worldIndex].ecs_world == NULL) continue;

    ECSTagID tag_id = ev_ecs_registertag(GameECSData.gameWorlds[worldIndex].ecs_world, tag_name);
    vec_push((vec_t*)&GameECSData.gameWorlds[worldIndex].tags, &tag_id);
    DEBUG_ASSERT(vec_len(GameECSData.gameTags) == vec_len(GameECSData.gameWorlds[worldIndex].tags));
  }
  return id;
}

U32 
ev_gameecs_setcomponent(
    ECSGameWorldHandle world,
    GameEntityID entt, 
    GameComponentID cmp,
    PTR data)
{
  ecs_set_ptr_w_entity(GameECSData.gameWorlds[world].ecs_world, entt, GameECSData.gameWorlds[world].components[cmp], GameECSData.gameComponents[cmp].size, data);
  return 0;
}

const PTR
ev_gameecs_getcomponent(
    ECSGameWorldHandle world_handle,
    GameEntityID entt,
    GameComponentID cmp)
{
  GameWorld world = GameECSData.gameWorlds[world_handle?world_handle:GameECSData.activeWorld];
  return ecs_get_w_entity(world.ecs_world, entt, world.components[cmp]);
}

PTR
ev_gameecs_getcomponentmut(
    ECSGameWorldHandle world_handle,
    GameEntityID entt,
    GameComponentID cmp)
{
  GameWorld world = GameECSData.gameWorlds[world_handle?world_handle:GameECSData.activeWorld];
  return ecs_get_mut_w_entity(world.ecs_world, entt, world.components[cmp], NULL);
}

bool
ev_gameecs_hascomponent(
    ECSGameWorldHandle world_handle,
    GameEntityID entt,
    GameComponentID cmp)
{
  GameWorld world = GameECSData.gameWorlds[world_handle?world_handle:GameECSData.activeWorld];
  return ecs_has_entity(world.ecs_world, entt, world.components[cmp]);
}

U32
ev_gameecs_addtag(
    ECSGameWorldHandle world_handle,
    GameEntityID entt,
    GameTagID tag)
{
  GameWorld world = GameECSData.gameWorlds[world_handle?world_handle:GameECSData.activeWorld];
  ecs_add_entity(world.ecs_world, entt, world.tags[tag]);

  return 0;
}

U32
ev_gameecs_addcomponent(
    ECSGameWorldHandle world_handle,
    GameEntityID entt,
    GameComponentID cmp)
{
  GameWorld world = GameECSData.gameWorlds[world_handle?world_handle:GameECSData.activeWorld];
  ecs_add_entity(world.ecs_world, entt, world.components[cmp]);
  return 0;
}

GameSystemID
ev_gameecs_registersystem(
    CONST_STR sig, 
    ECSPipelineStage stage, 
    FN_PTR fn, 
    CONST_STR name)
{
  GameSystemID id = vec_push((vec_t*)&GameECSData.gameSystems, &(GameSystem) {
      .name = name,
      .stage = stage,
      .fn = fn,
      .sig = sig
  });

  for(size_t worldIndex = 1; worldIndex < vec_len(GameECSData.gameWorlds); worldIndex++) {
    if(GameECSData.gameWorlds[worldIndex].ecs_world == NULL) continue;

    ECSSystemID sys_id = ev_ecs_registersystem(GameECSData.gameWorlds[worldIndex].ecs_world, sig, GameECSData.gameWorlds[worldIndex].pipeline_stages[stage], fn, name);
    vec_push((vec_t*)&GameECSData.gameWorlds[worldIndex].systems, &sys_id);
    DEBUG_ASSERT(vec_len(GameECSData.gameSystems) == vec_len(GameECSData.gameWorlds[worldIndex].systems));
  }
  return id;
}

void
ev_gameecs_setsystemrate(
    GameSystemID system,
    F32 rate)
{
  GameECSData.gameSystems[system].rate = rate;

  for(size_t worldIndex = 1; worldIndex < vec_len(GameECSData.gameWorlds); worldIndex++) {
    if(GameECSData.gameWorlds[worldIndex].ecs_world == NULL) continue;

    ev_ecs_setsystemrate(
        GameECSData.gameWorlds[worldIndex].ecs_world, 
        GameECSData.gameWorlds[worldIndex].systems[system], 
        rate);
  }

}

void
ev_gameecs_setonaddtrigger(
    CONST_STR trigger_name,
    CONST_STR cmp_name,
    void(*onaddfn)(ECSQuery))
{  
  vec_push((vec_t*)&GameECSData.gameTriggers, &(GameTrigger) {
      .trigger_name = trigger_name,
      .cmp_name = cmp_name,
      .fn = onaddfn,
      .type = OnAddTrigger
  });

  for(size_t worldIndex = 1; worldIndex < vec_len(GameECSData.gameWorlds); worldIndex++) {
    if(GameECSData.gameWorlds[worldIndex].ecs_world == NULL) continue;

    ev_ecs_setonaddtrigger(GameECSData.gameWorlds[worldIndex].ecs_world, trigger_name, cmp_name, onaddfn);
  }
}

void
ev_gameecs_setonremovetrigger(
    CONST_STR trigger_name,
    CONST_STR cmp_name,
    void(*onremovefn)(ECSQuery))
{  
  vec_push((vec_t*)&GameECSData.gameTriggers, &(GameTrigger) {
      .trigger_name = trigger_name,
      .cmp_name = cmp_name,
      .fn = onremovefn,
      .type = OnRemoveTrigger
  });

  for(size_t worldIndex = 1; worldIndex < vec_len(GameECSData.gameWorlds); worldIndex++) {
    if(GameECSData.gameWorlds[worldIndex].ecs_world == NULL) continue;

    ev_ecs_setonremovetrigger(GameECSData.gameWorlds[worldIndex].ecs_world, trigger_name, cmp_name, onremovefn);
  }

}

void
ev_gameecs_setonsettrigger(
    CONST_STR trigger_name,
    CONST_STR cmp_name,
    void(*onsetfn)(ECSQuery))
{  
  vec_push((vec_t*)&GameECSData.gameTriggers, &(GameTrigger) {
      .trigger_name = trigger_name,
      .cmp_name = cmp_name,
      .fn = onsetfn,
      .type = OnSetTrigger
  });

  for(size_t worldIndex = 1; worldIndex < vec_len(GameECSData.gameWorlds); worldIndex++) {
    if(GameECSData.gameWorlds[worldIndex].ecs_world == NULL) continue;

    ev_ecs_setonsettrigger(GameECSData.gameWorlds[worldIndex].ecs_world, trigger_name, cmp_name, onsetfn);
  }
}

bool
ev_gameecs_hastag(
    ECSGameWorldHandle world_handle,
    GameEntityID entt,
    GameTagID tag)
{
  GameWorld world = GameECSData.gameWorlds[world_handle];
  return ecs_has_entity(world.ecs_world, entt, world.tags[tag]);
}

void
ev_gameecs_foreachchild(
    ECSGameWorldHandle world_handle,
    GameEntityID entt,
    void(*iter_fn)(ECSGameWorldHandle, GameEntityID))
{
  GameWorld world = GameECSData.gameWorlds[world_handle];

  ecs_iter_t it = ecs_scope_iter(world.ecs_world, entt);
  while(ecs_scope_next(&it)) {
    for (int i = 0; i < it.count; i ++) {
      iter_fn(world_handle, it.entities[i]);
    }
  }
}

GameEntityID
ev_gameecs_getparent(
    ECSGameWorldHandle world_handle,
    GameEntityID entt)
{
  return ecs_get_parent_w_entity(GameECSData.gameWorlds[world_handle].ecs_world, entt, 0);
}

void
ev_gameecs_removetag(
    ECSGameWorldHandle world_handle,
    GameEntityID entt,
    GameTagID tag)
{
  GameWorld world = GameECSData.gameWorlds[world_handle];
  ecs_remove_entity(world.ecs_world, entt, world.tags[tag]);
}

void
ev_gameecs_setsingleton(
    ECSGameWorldHandle world_handle,
    GameComponentID comp,
    PTR data)
{
  GameWorld world = GameECSData.gameWorlds[world_handle];
  ECSComponentID cmp = world.components[comp];
  size_t cmp_size = GameECSData.gameComponents[comp].size;
  ev_ecs_setcomponent(world.ecs_world, cmp, cmp, cmp_size, data);
  ev_ecs_modified(world.ecs_world, cmp, cmp);
}

PTR
ev_gameecs_getsingleton(
    ECSGameWorldHandle world_handle,
    GameComponentID comp)
{
  GameWorld world = GameECSData.gameWorlds[world_handle];
  ECSComponentID cmp = world.components[comp];
  return ev_ecs_getcomponent(world.ecs_world, cmp, cmp);
}

void
ev_gameecs_modified(
    ECSGameWorldHandle world_handle,
    GameEntityID entt,
    GameComponentID comp)
{
  GameWorld world = GameECSData.gameWorlds[world_handle];
  ev_ecs_modified(world.ecs_world, entt, world.components[comp]);
}

GameEntityID
ev_gameecs_createentity(
    ECSGameWorldHandle world_handle)
{
  return ecs_new(GameECSData.gameWorlds[world_handle].ecs_world, 0);
}


void
ev_gameecs_destroyentity(
    ECSGameWorldHandle world_handle,
    GameEntityID entt)
{
  GameWorld world = GameECSData.gameWorlds[world_handle];
  ecs_delete(world.ecs_world, entt);
}

GameEntityID
ev_gameecs_createchildentity(
    ECSGameWorldHandle world_handle,
    GameEntityID parent)
{
  GameWorld world = GameECSData.gameWorlds[world_handle];
  return ecs_new_w_entity(world.ecs_world, ECS_CHILDOF | parent);
}

void
ev_gameecs_addchildtoentity(
    ECSGameWorldHandle world_handle,
    GameEntityID parent,
    GameEntityID child)
{
  GameWorld world = GameECSData.gameWorlds[world_handle];
  ecs_add_entity(world.ecs_world, child, ECS_CHILDOF | parent);
}

void
ev_gameecs_destroyworld(
    ECSGameWorldHandle world_handle)
{
  GameWorld *world = &GameECSData.gameWorlds[world_handle];
  if(world->ecs_world) {
    ecs_fini(world->ecs_world);
    world->ecs_world = NULL;
  }
  if(world->components) {
    vec_fini(world->components);
    world->components = NULL;
  }
  if(world->tags) {
    vec_fini(world->tags);
    world->tags = NULL;
  }
  if(world->systems) {
    vec_fini(world->systems);
    world->systems = NULL;
  }
}

U32
ev_gameecs_progress(
    ECSGameWorldHandle world_handle,
    F32 deltaTime)
{
  GameWorld world = GameECSData.gameWorlds[world_handle];
  return ecs_progress(world.ecs_world, deltaTime)?0:1;
}

void
ev_gameecs_setentityname(
    ECSGameWorldHandle world_handle,
    GameEntityID entt,
    const char *name)
{
  GameWorld world = GameECSData.gameWorlds[world_handle];
  ecs_set(world.ecs_world, entt, EcsName, {.alloc_value=(char*)name});
}

const char *
ev_gameecs_getentityname(
    ECSGameWorldHandle world_handle,
    GameEntityID entt)
{
  GameWorld world = GameECSData.gameWorlds[world_handle];
  return ecs_get_name(world.ecs_world, entt);
}

GameEntityID
ev_gameecs_getentityfromname(
    ECSGameWorldHandle world_handle,
    const char *name)
{
  GameWorld world = GameECSData.gameWorlds[world_handle];
  return ecs_lookup_fullpath(world.ecs_world, name);
}

GameEntityID
ev_gameecs_getchildfromname(
    ECSGameWorldHandle world_handle,
    GameEntityID parent,
    const char *name)
{
  GameWorld world = GameECSData.gameWorlds[world_handle];
  return ecs_lookup_path(world.ecs_world, parent, name);
}
