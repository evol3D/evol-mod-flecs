#define EV_MODULE_DEFINE
#include <evol/evolmod.h>

#include <flecs.h>

struct ECSData {
  ecs_world_t *activeScene;
} ECSData;


EV_CONSTRUCTOR 
{
  static_assert(sizeof(ECSEntityID) == sizeof(ecs_entity_t), "Size of ECSEntityID != Size of ecs_entity_t");
  static_assert(sizeof(ECSComponentID) == sizeof(ecs_entity_t), "Size of ECSComponentID != Size of ecs_entity_t");

  ECSData.activeScene = NULL;

  ecs_os_set_api_defaults();
  ecs_os_api_t api = ecs_os_api;

  // TODO get ev_flecs_init_os_api from evol-legacy
  /* ev_flecs_init_os_api(&api); */

  ecs_os_set_api(&api);

  return 0;
}

U32 _ev_ecs_update(F32 deltaTime)
{
  DEBUG_ASSERT(ECSData.activeScene);

  bool result = ecs_progress(ECSData.activeScene, deltaTime);

  return result?0:1;
}

U32 _ev_ecs_newscene()
{
  DEBUG_ASSERT(!ECSData.activeScene);

  ECSData.activeScene = ecs_init();

  // TODO enable after initializing the OS API
  /* ecs_enable_locking(ECSData.activeScene, 1); */
}

U32 _ev_ecs_unlock()
{
  /* ecs_unlock(ECSData.activeScene); */
  return 0;
}

U32 _ev_ecs_lock()
{
  /* ecs_lock(ECSData.activeScene); */
  return 0;
}

ECSComponentID _ev_ecs_registercomponent(CONST_STR cmp_name, U32 cmp_size, U32 cmp_align)
{
  return ecs_new_component(ECSData.activeScene, 0, cmp_name, cmp_size, cmp_align);
}

ECSEntityID _ev_ecs_createentity()
{
  return ecs_new(ECSData.activeScene, 0);
}

EV_DESTRUCTOR 
{
  if(ECSData.activeScene)
  {
    ecs_fini(ECSData.activeScene);
  }

  return 0;
}

U32 _ev_ecs_addcomponent(ECSEntityID entt, ECSComponentID cmp, U32 cmp_size, PTR data)
{
  ecs_set_ptr_w_entity(ECSData.activeScene, entt, cmp, cmp_size, data);
  return 0;
}

EV_BINDINGS
{
  EV_NS_BIND_FN(ECS, update, _ev_ecs_update);

  EV_NS_BIND_FN(ECS, registerComponent, _ev_ecs_registercomponent);

  EV_NS_BIND_FN(ECS, createEntity, _ev_ecs_createentity);
  EV_NS_BIND_FN(ECS, addComponent, _ev_ecs_addcomponent);

  EV_NS_BIND_FN(ECS, newScene, _ev_ecs_newscene);

  EV_NS_BIND_FN(ECS, lock, _ev_ecs_lock);
  EV_NS_BIND_FN(ECS, unlock, _ev_ecs_unlock);
}
