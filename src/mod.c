#define EV_MODULE_DEFINE
#include <evol/evolmod.h>

#include <os_api.h>

#include <flecs.h>

struct ECSData {
  ecs_world_t *activeScene;
  ecs_entity_t pipelineStages[EV_ECS_PIPELINE_STAGE_COUNT];
} ECSData;

EV_CONSTRUCTOR 
{
  static_assert(sizeof(ECSEntityID) == sizeof(ecs_entity_t), "Size of ECSEntityID != Size of ecs_entity_t");
  static_assert(sizeof(ECSComponentID) == sizeof(ecs_entity_t), "Size of ECSComponentID != Size of ecs_entity_t");

  ECSData.activeScene = NULL;

  ecs_os_set_api_defaults();
  ecs_os_api_t api = ecs_os_api;

  ev_flecs_init_os_api(&api);

  ecs_os_set_api(&api);

  return 0;
}

U32 _ev_ecs_update(F32 deltaTime)
{
  DEBUG_ASSERT(ECSData.activeScene);

  bool result = ecs_progress(ECSData.activeScene, deltaTime);

  return result?0:1;
}

void _ev_ecs_init_pipeline()
{
  #define STAGE(name) EV_CONCAT(name, _IMPL)

  ECS_TAG(ECSData.activeScene, EV_ECS_PIPELINE_STAGE_PREUPDATE_IMPL);
  ECSData.pipelineStages[EV_ECS_PIPELINE_STAGE_PREUPDATE] = STAGE(EV_ECS_PIPELINE_STAGE_PREUPDATE);

  ECS_TAG(ECSData.activeScene, EV_ECS_PIPELINE_STAGE_UPDATE_IMPL);
  ECSData.pipelineStages[EV_ECS_PIPELINE_STAGE_UPDATE] = STAGE(EV_ECS_PIPELINE_STAGE_UPDATE);

  ECS_TAG(ECSData.activeScene, EV_ECS_PIPELINE_STAGE_POSTUPDATE_IMPL);
  ECSData.pipelineStages[EV_ECS_PIPELINE_STAGE_POSTUPDATE] = STAGE(EV_ECS_PIPELINE_STAGE_POSTUPDATE);

  ECS_PIPELINE(ECSData.activeScene, evECSPipeline, 
    EV_ECS_PIPELINE_STAGE_PREUPDATE_IMPL,
    EV_ECS_PIPELINE_STAGE_UPDATE_IMPL,
    EV_ECS_PIPELINE_STAGE_POSTUPDATE_IMPL
  );

  ecs_set_pipeline(ECSData.activeScene, evECSPipeline);

  #undef STAGE
}

U32 _ev_ecs_newscene()
{
  DEBUG_ASSERT(!ECSData.activeScene);

  ECSData.activeScene = ecs_init();

  _ev_ecs_init_pipeline();

  ecs_enable_locking(ECSData.activeScene, 1);
}

U32 _ev_ecs_unlock()
{
  ecs_unlock(ECSData.activeScene);
  return 0;
}

U32 _ev_ecs_lock()
{
  ecs_lock(ECSData.activeScene);
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

U32 _ev_ecs_enableentity(ECSEntityID entt)
{
  ecs_enable(ECSData.activeScene, entt, true);
  return 0;
}

U32 _ev_ecs_disableentity(ECSEntityID entt)
{
  ecs_enable(ECSData.activeScene, entt, false);
  return 0;
}

U32 _ev_ecs_registersystem(CONST_STR sig, ECSPipelineStage stage, FN_PTR fn, CONST_STR name)
{
  ecs_new_system(ECSData.activeScene, 0, name, ECSData.pipelineStages[stage], sig, (ecs_iter_action_t)fn);
  return 0;
}

PTR _ev_ecs_getquerycolumn(ECSQuery query, U32 size, U32 idx)
{
  ecs_iter_t *iter = (ecs_iter_t *)query;
  return ecs_column_w_size(iter, size, idx);
}

U32 _ev_ecs_getquerymatchcount(ECSQuery query)
{
  return ((ecs_iter_t *)query)->count;
}

EV_BINDINGS
{
  EV_NS_BIND_FN(ECS, update, _ev_ecs_update);

  EV_NS_BIND_FN(ECS, registerComponent, _ev_ecs_registercomponent);
  EV_NS_BIND_FN(ECS, registerSystem, _ev_ecs_registersystem);

  EV_NS_BIND_FN(ECS, getQueryColumn, _ev_ecs_getquerycolumn);
  EV_NS_BIND_FN(ECS, getQueryMatchCount, _ev_ecs_getquerymatchcount);

  EV_NS_BIND_FN(ECS, enableEntity, _ev_ecs_enableentity);
  EV_NS_BIND_FN(ECS, disableEntity, _ev_ecs_disableentity);
  EV_NS_BIND_FN(ECS, createEntity, _ev_ecs_createentity);
  EV_NS_BIND_FN(ECS, addComponent, _ev_ecs_addcomponent);

  EV_NS_BIND_FN(ECS, newScene, _ev_ecs_newscene);

  EV_NS_BIND_FN(ECS, lock, _ev_ecs_lock);
  EV_NS_BIND_FN(ECS, unlock, _ev_ecs_unlock);
}
