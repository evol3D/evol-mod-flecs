#define EV_ECS_TYPES_INCLUDE
#include <common_ecs.h>

ECSTagID
ev_ecs_registertag(
    ecs_world_t *world,
    CONST_STR tag_name)
{
  ecs_entity_t tag = ecs_new_entity(world, 0, tag_name, "0");
  struct {
      union {
          ecs_vector_t vector;
          uint64_t align;
      } header;
      ecs_entity_t array[1];
  } tag_value = {
      .header.vector = {
          .elem_size = (int32_t)(ECS_SIZEOF(ecs_entity_t)),
          .count = 1,
          .size = 1
      }
  };
  const ecs_vector_t *FLECS__Ttag = (ecs_vector_t*)&tag_value;
  ecs_os_memcpy(tag_value.array, &tag, sizeof(ecs_entity_t) * 1);
  (void)FLECS__Ttag;

  return tag;
}

U32 
ev_ecs_setcomponent(
    ecs_world_t *world,
    ECSEntityID entt, 
    ECSComponentID cmp,
    U32 cmp_size,
    PTR data)
{
  ecs_set_ptr_w_entity(world, entt, cmp, cmp_size, data);
  return 0;
}

U32 
ev_ecs_addcomponent(
    ecs_world_t *world,
    ECSEntityID entt, 
    ECSComponentID cmp)
{
  ecs_add_entity(world, entt, cmp);
  return 0;
}

U32
ev_ecs_addtag(
    ecs_world_t *world,
    ECSEntityID entt,
    ECSTagID tag)
{
  ecs_add_entity(world, entt, tag);
  return 0;
}

bool
ev_ecs_hastag(
    ecs_world_t *world,
    ECSEntityID entt,
    ECSTagID tag)
{
  return ecs_has_entity(world, entt, tag);
}

U32 
ev_ecs_enableentity(
    ecs_world_t *world,
    ECSEntityID entt)
{
  ecs_enable(world, entt, true);
  return 0;
}

U32 
ev_ecs_disableentity(
    ecs_world_t *world,
    ECSEntityID entt)
{
  ecs_enable(world, entt, false);
  return 0;
}

ECSEntityID
ev_ecs_getparent(
    ecs_world_t *world,
    ECSEntityID entt)
{
  return ecs_get_parent_w_entity(world, entt, 0);
}

ECSSystemID 
ev_ecs_registersystem(
    ecs_world_t *world,
    CONST_STR sig, 
    ecs_entity_t stage, 
    FN_PTR fn, 
    CONST_STR name)
{
  ecs_iter_action_t action = (ecs_iter_action_t)fn;
  ECSSystemID newSystem = ecs_new_system(world, 0, name, stage, sig, action);
  return newSystem;
}

void
ev_ecs_setsystemrate(
    ecs_world_t *world,
    ECSSystemID system,
    F32 rate)
{
  ecs_set_interval(world, system, 1.f/rate);
}

PTR
ev_ecs_getquerycolumn(
    ECSQuery query, 
    U32 size, 
    U32 idx)
{
  ecs_iter_t *iter = (ecs_iter_t *)query;
  return ecs_column_w_size(iter, size, idx);
}

U32
ev_ecs_getquerymatchcount(
    ECSQuery query)
{
  return ((ecs_iter_t *)query)->count;
}

ECSEntityID *
ev_ecs_getqueryentities(
    ECSQuery query)
{
  return ((ecs_iter_t *)query)->entities;
}

void
ev_ecs_modified(
    ecs_world_t *world,
    ECSEntityID entt,
    ECSComponentID comp)
{
  ecs_modified_w_entity(world, entt, comp);
}

// Assumes that the component is always mutable. Easier for scripting?
PTR
ev_ecs_getcomponent(
    ecs_world_t *world,
    ECSEntityID entt,
    ECSComponentID cmp)
{
  return ecs_get_mut_w_entity(world, entt, cmp, NULL);
}

bool
ev_ecs_hascomponent(
    ecs_world_t *world,
    ECSEntityID entt,
    ECSComponentID cmp)
{
  return ecs_has_entity(world, entt, cmp);
}

void
ev_ecs_setsingleton(
    ecs_world_t *world,
    ECSComponentID cmp,
    U32 cmp_size,
    PTR data)
{
  ev_ecs_setcomponent(world, cmp, cmp, cmp_size, data);
  ev_ecs_modified(world, cmp, cmp);
}

PTR
ev_ecs_getsingleton(
    ecs_world_t *world,
    ECSComponentID cmp)
{
  return ev_ecs_getcomponent(world, cmp, cmp);
}

void
ev_ecs_foreachchild(
    ecs_world_t *world,
    ECSEntityID entt,
    void(*iter_fn)(ECSEntityID))
{
  ecs_iter_t it = ecs_scope_iter(world, entt);

  while(ecs_scope_next(&it)) {
    for (int i = 0; i < it.count; i ++) {
      iter_fn(it.entities[i]);
    }
  }
}

void
ev_ecs_removetag(
    ecs_world_t *world,
    ECSEntityID entt,
    ECSTagID tag)
{
  ecs_remove_entity(world, entt, tag);
}

void
ev_ecs_setonaddtrigger(
    ecs_world_t *world,
    CONST_STR trigger_name,
    CONST_STR cmp_name,
    void(*onaddfn)(ECSQuery))
{
  ecs_new_trigger(world, 0, trigger_name, EcsOnAdd, cmp_name, onaddfn); 
}

void
ev_ecs_setonremovetrigger(
    ecs_world_t *world,
    CONST_STR trigger_name,
    CONST_STR cmp_name,
    void(*onremovefn)(ECSQuery))
{
  ecs_new_trigger(world, 0, trigger_name, EcsOnRemove, cmp_name, onremovefn); 
}

void
ev_ecs_setonsettrigger(
    ecs_world_t *world,
    CONST_STR trigger_name,
    CONST_STR cmp_name,
    void(*onsetfn)(ECSQuery))
{
  ecs_new_system(world, 0, trigger_name, EcsOnSet, cmp_name, onsetfn);
}

U32 
ev_ecs_unlock(
    ecs_world_t *world)
{
  ecs_unlock(world);
  return 0;
}

U32 
ev_ecs_lock(
    ecs_world_t *world)
{
  ecs_lock(world);
  return 0;
}

ECSComponentID 
ev_ecs_registercomponent(
    ecs_world_t *world,
    CONST_STR cmp_name, 
    U32 cmp_size, 
    U32 cmp_align)
{
  return ecs_new_component(world, 0, cmp_name, cmp_size, cmp_align);
}

ECSEntityID 
ev_ecs_createentity(
    ecs_world_t *world)
{
  return ecs_new(world, 0);
}

void
ev_ecs_deleteentity(
    ecs_world_t *world,
    ECSEntityID entt)
{
  ecs_delete(world, entt);
}

void
ev_ecs_addchild(
    ecs_world_t *world,
    ECSEntityID parent,
    ECSEntityID child)
{
  ecs_add_entity(world, child, ECS_CHILDOF | parent);
}

ECSEntityID
ev_ecs_createchild(
    ecs_world_t *world,
    ECSEntityID parent)
{
  return ecs_new_w_entity(world, ECS_CHILDOF | parent);
}
