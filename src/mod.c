#define EV_MODULE_DEFINE
#include <evol/evolmod.h>

#include <os_api.h>

#include <flecs.h>

struct ECSData {
  ecs_world_t *activeScene;
  ecs_entity_t pipelineStages[EV_ECS_PIPELINE_STAGE_COUNT];
} ECSData;

void init_scripting_api();

EV_CONSTRUCTOR 
{
  static_assert(sizeof(ECSEntityID)    == sizeof(ecs_entity_t), "Size of ECSEntityID != Size of ecs_entity_t");
  static_assert(sizeof(ECSComponentID) == sizeof(ecs_entity_t), "Size of ECSComponentID != Size of ecs_entity_t");
  static_assert(sizeof(ECSSystemID)    == sizeof(ecs_entity_t), "Size of ECSComponentID != Size of ecs_entity_t");

  ECSData.activeScene = NULL;

  ecs_os_set_api_defaults();
  ecs_os_api_t api = ecs_os_api;

  ev_flecs_init_os_api(&api);

  ecs_os_set_api(&api);

  init_scripting_api();

  return 0;
}

EVMODAPI U32 
_ev_ecs_update(F32 deltaTime)
{
  DEBUG_ASSERT(ECSData.activeScene);

  bool result = ecs_progress(ECSData.activeScene, deltaTime);

  return result?0:1;
}

EVMODAPI ECSTagID
_ev_ecs_registertag(
    CONST_STR tag_name)
{
  ecs_entity_t tag = ecs_new_entity(ECSData.activeScene, 0, tag_name, "0");
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

// The following section is left commented until a but in flecs is fixed. The
// but is as follow: "timers don't tick in custom pipelines"
/* // NOTICE! */
/* // Adding a stage in here adds requires the addition of this stage in the enum */
/* // exposed by `evmod.types` */
/* #define PIPELINE_STAGES() \ */
/*   PIPELINE_OP(PREUPDATE)  \ */
/*   PIPELINE_OP(UPDATE)     \ */
/*   PIPELINE_OP(POSTUPDATE) */

/* EVMODAPI void */ 
/* _ev_ecs_init_pipeline() */
/* { */
/* #define STAGE(name) EV_CONCAT(EV_ECS_PIPELINE_STAGE_, name) */
/* #define STAGE_IMPL(name) EV_CONCAT(STAGE(name), _IMPL) */
/* #define STAGE_IMPL_STR(name) EV_STRINGIZE(STAGE_IMPL(name)) */

/*   // Registering stage tags in the world */
/* #define PIPELINE_OP(stage) \ */
/*   ECSEntityID STAGE_IMPL(stage) = _ev_ecs_registertag(STAGE_IMPL_STR(stage)); */
/*   PIPELINE_STAGES() */
/* #undef PIPELINE_OP */

/*   // Saving stage IDs in enum */
/* #define PIPELINE_OP(stage) \ */
/*   ECSData.pipelineStages[STAGE(stage)] = STAGE_IMPL(stage); */
/*   PIPELINE_STAGES() */
/* #undef PIPELINE_OP */

/*   ECS_PIPELINE(ECSData.activeScene, evECSPipeline, */ 
/*     EV_ECS_PIPELINE_STAGE_PREUPDATE_IMPL, */
/*     EV_ECS_PIPELINE_STAGE_UPDATE_IMPL, */
/*     EV_ECS_PIPELINE_STAGE_POSTUPDATE_IMPL */
/*   ); */

/*   ecs_set_pipeline(ECSData.activeScene, evECSPipeline); */

/* #undef STAGE_IMPL_STR */
/* #undef STAGE_IMPL */
/* #undef STAGE */
/* } */

EVMODAPI void 
_ev_ecs_init_pipeline()
{
  ECSData.pipelineStages[EV_ECS_PIPELINE_STAGE_PREUPDATE] = EcsPreUpdate;
  ECSData.pipelineStages[EV_ECS_PIPELINE_STAGE_UPDATE] = EcsOnUpdate;
  ECSData.pipelineStages[EV_ECS_PIPELINE_STAGE_POSTUPDATE] = EcsPostUpdate;
}

EVMODAPI U32 
_ev_ecs_newscene()
{
  DEBUG_ASSERT(!ECSData.activeScene);

  ECSData.activeScene = ecs_init();

  _ev_ecs_init_pipeline();

  ecs_enable_locking(ECSData.activeScene, 1);
}

EVMODAPI U32 
_ev_ecs_unlock()
{
  ecs_unlock(ECSData.activeScene);
  return 0;
}

EVMODAPI U32 
_ev_ecs_lock()
{
  ecs_lock(ECSData.activeScene);
  return 0;
}

EVMODAPI ECSComponentID 
_ev_ecs_registercomponent(
    CONST_STR cmp_name, 
    U32 cmp_size, 
    U32 cmp_align)
{
  return ecs_new_component(ECSData.activeScene, 0, cmp_name, cmp_size, cmp_align);
}

EVMODAPI ECSEntityID 
_ev_ecs_createentity()
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

EVMODAPI U32 
_ev_ecs_addcomponent(
    ECSEntityID entt, 
    ECSComponentID cmp, 
    U32 cmp_size, 
    PTR data)
{
  ecs_set_ptr_w_entity(ECSData.activeScene, entt, cmp, cmp_size, data);
  return 0;
}

EVMODAPI U32
_ev_ecs_addtag(
    ECSEntityID entt,
    ECSTagID tag)
{
  ecs_add_entity(ECSData.activeScene, entt, tag);
  return 0;
}

EVMODAPI U32 
_ev_ecs_enableentity(
    ECSEntityID entt)
{
  ecs_enable(ECSData.activeScene, entt, true);
  return 0;
}

EVMODAPI U32 
_ev_ecs_disableentity(
    ECSEntityID entt)
{
  ecs_enable(ECSData.activeScene, entt, false);
  return 0;
}

EVMODAPI ECSSystemID 
_ev_ecs_registersystem(
    CONST_STR sig, 
    ECSPipelineStage stage, 
    FN_PTR fn, 
    CONST_STR name)
{
  ecs_iter_action_t action = (ecs_iter_action_t)fn;
  ECSSystemID newSystem = ecs_new_system(ECSData.activeScene, 0, name, ECSData.pipelineStages[stage], sig, action);
  return newSystem;
}

EVMODAPI void
_ev_ecs_setsystemrate(
    ECSSystemID system,
    F32 rate)
{
  ecs_set_interval(ECSData.activeScene, system, 1.f/rate);
}

EVMODAPI PTR
_ev_ecs_getquerycolumn(
    ECSQuery query, 
    U32 size, 
    U32 idx)
{
  ecs_iter_t *iter = (ecs_iter_t *)query;
  return ecs_column_w_size(iter, size, idx);
}

EVMODAPI U32
_ev_ecs_getquerymatchcount(
    ECSQuery query)
{
  return ((ecs_iter_t *)query)->count;
}

EVMODAPI ECSEntityID *
_ev_ecs_getqueryentities(
    ECSQuery query)
{
  return ((ecs_iter_t *)query)->entities;
}

// Assumes that the component is always mutable. Easier for scripting?
EVMODAPI PTR
_ev_ecs_getcomponent(
    ECSEntityID entt,
    ECSComponentID cmp)
{
  return ecs_get_mut_w_entity(ECSData.activeScene, entt, cmp, NULL);
}

EV_BINDINGS
{
  EV_NS_BIND_FN(ECS, update, _ev_ecs_update);

  EV_NS_BIND_FN(ECS, registerComponent, _ev_ecs_registercomponent);
  EV_NS_BIND_FN(ECS, registerSystem, _ev_ecs_registersystem);
  EV_NS_BIND_FN(ECS, registerTag, _ev_ecs_registertag);

  EV_NS_BIND_FN(ECS, setSystemRate, _ev_ecs_setsystemrate);

  EV_NS_BIND_FN(ECS, getQueryColumn, _ev_ecs_getquerycolumn);
  EV_NS_BIND_FN(ECS, getQueryMatchCount, _ev_ecs_getquerymatchcount);
  EV_NS_BIND_FN(ECS, getQueryEntities, _ev_ecs_getqueryentities);

  EV_NS_BIND_FN(ECS, createEntity, _ev_ecs_createentity);
  EV_NS_BIND_FN(ECS, enableEntity, _ev_ecs_enableentity);
  EV_NS_BIND_FN(ECS, disableEntity, _ev_ecs_disableentity);

  EV_NS_BIND_FN(ECS, addComponent, _ev_ecs_addcomponent);
  EV_NS_BIND_FN(ECS, addTag, _ev_ecs_addtag);

  EV_NS_BIND_FN(ECS, getComponent, _ev_ecs_getcomponent);

  EV_NS_BIND_FN(ECS, newScene, _ev_ecs_newscene);

  EV_NS_BIND_FN(ECS, lock, _ev_ecs_lock);
  EV_NS_BIND_FN(ECS, unlock, _ev_ecs_unlock);
}

// Initializing the scripting API
#define TYPE_MODULE evmod_script
#define NAMESPACE_MODULE evmod_script
#include <evol/meta/type_import.h>
#include <evol/meta/namespace_import.h>

void init_scripting_api()
{
  evolmodule_t scripting_module = evol_loadmodule("script");
  if(!scripting_module) return;
  IMPORT_NAMESPACE(ScriptInterface, scripting_module);

  // Do stuff

  evol_unloadmodule(scripting_module);
  // Invalidating namespace reference as the module is unloaded
  ScriptInterface = NULL;
}
