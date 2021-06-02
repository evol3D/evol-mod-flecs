#define EV_MODULE_DEFINE
#include <evol/evolmod.h>

#include <os_api.h>
#include <flecs.h>

#include <game_ecs.h>
#include <asset_ecs.h>

EV_CONSTRUCTOR 
{
  static_assert(sizeof(GenericHandle) == sizeof(ecs_entity_t), "Size of GenericHandle != Size of ecs_entity_t");

  ecs_os_set_api_defaults();
  ecs_os_api_t api = ecs_os_api;
  ev_flecs_init_os_api(&api);
  ecs_os_set_api(&api);

  ev_gameecs_init();
  ev_assetecs_init();

  return 0;
}

EV_DESTRUCTOR 
{
  ev_assetecs_deinit();
  ev_gameecs_deinit();
  return 0;
}

EV_BINDINGS
{
  EV_NS_BIND_FN(ECS, getQueryColumn    , ev_ecs_getquerycolumn);
  EV_NS_BIND_FN(ECS, getQueryMatchCount, ev_ecs_getquerymatchcount);
  EV_NS_BIND_FN(ECS, getQueryEntities  , ev_ecs_getqueryentities);

  EV_NS_BIND_FN(GameECS, newWorld          , ev_gameecs_newworld);
  EV_NS_BIND_FN(GameECS, destroyWorld      , ev_gameecs_destroyworld);
  EV_NS_BIND_FN(GameECS, progress          , ev_gameecs_progress);
  EV_NS_BIND_FN(GameECS, createEntity      , ev_gameecs_createentity);
  EV_NS_BIND_FN(GameECS, destroyEntity     , ev_gameecs_destroyentity);
  EV_NS_BIND_FN(GameECS, registerComponent , ev_gameecs_registercomponent);
  EV_NS_BIND_FN(GameECS, getComponent      , ev_gameecs_getcomponent);
  EV_NS_BIND_FN(GameECS, setComponent      , ev_gameecs_setcomponent);
  EV_NS_BIND_FN(GameECS, hasComponent      , ev_gameecs_hascomponent);
  EV_NS_BIND_FN(GameECS, addComponent      , ev_gameecs_addcomponent);
  EV_NS_BIND_FN(GameECS, addTag            , ev_gameecs_addtag);
  EV_NS_BIND_FN(GameECS, hasTag            , ev_gameecs_hastag);
  EV_NS_BIND_FN(GameECS, removeTag         , ev_gameecs_removetag);
  EV_NS_BIND_FN(GameECS, registerTag       , ev_gameecs_registertag);
  EV_NS_BIND_FN(GameECS, registerSystem    , ev_gameecs_registersystem);
  EV_NS_BIND_FN(GameECS, setSystemRate     , ev_gameecs_setsystemrate);
  EV_NS_BIND_FN(GameECS, setOnAddTrigger   , ev_gameecs_setonaddtrigger);
  EV_NS_BIND_FN(GameECS, setOnRemoveTrigger, ev_gameecs_setonremovetrigger);
  EV_NS_BIND_FN(GameECS, setOnSetTrigger   , ev_gameecs_setonsettrigger);
  EV_NS_BIND_FN(GameECS, forEachChild      , ev_gameecs_foreachchild);
  EV_NS_BIND_FN(GameECS, getParent         , ev_gameecs_getparent);
  EV_NS_BIND_FN(GameECS, setSingleton      , ev_gameecs_setsingleton);
  EV_NS_BIND_FN(GameECS, getSingleton      , ev_gameecs_getsingleton);
  EV_NS_BIND_FN(GameECS, modified          , ev_gameecs_modified);
  EV_NS_BIND_FN(GameECS, createChildEntity , ev_gameecs_createchildentity);
  EV_NS_BIND_FN(GameECS, addChildToEntity  , ev_gameecs_addchildtoentity);
  EV_NS_BIND_FN(GameECS, setEntityName     , ev_gameecs_setentityname);
  EV_NS_BIND_FN(GameECS, getEntityName     , ev_gameecs_getentityname);
  EV_NS_BIND_FN(GameECS, getEntityFromName , ev_gameecs_getentityfromname);
  EV_NS_BIND_FN(GameECS, getChildFromName  , ev_gameecs_getchildfromname);

  EV_NS_BIND_FN(AssetECS, newWorld          , ev_assetecs_newworld);
  EV_NS_BIND_FN(AssetECS, destroyWorld      , ev_assetecs_destroyworld);
  EV_NS_BIND_FN(AssetECS, progress          , ev_assetecs_progress);
  EV_NS_BIND_FN(AssetECS, createEntity      , ev_assetecs_createentity);
  EV_NS_BIND_FN(AssetECS, destroyEntity     , ev_assetecs_destroyentity);
  EV_NS_BIND_FN(AssetECS, registerComponent , ev_assetecs_registercomponent);
  EV_NS_BIND_FN(AssetECS, getComponent      , ev_assetecs_getcomponent);
  EV_NS_BIND_FN(AssetECS, setComponent      , ev_assetecs_setcomponent);
  EV_NS_BIND_FN(AssetECS, hasComponent      , ev_assetecs_hascomponent);
  EV_NS_BIND_FN(AssetECS, addComponent      , ev_assetecs_addcomponent);
  EV_NS_BIND_FN(AssetECS, addTag            , ev_assetecs_addtag);
  EV_NS_BIND_FN(AssetECS, hasTag            , ev_assetecs_hastag);
  EV_NS_BIND_FN(AssetECS, removeTag         , ev_assetecs_removetag);
  EV_NS_BIND_FN(AssetECS, registerTag       , ev_assetecs_registertag);
  EV_NS_BIND_FN(AssetECS, registerSystem    , ev_assetecs_registersystem);
  EV_NS_BIND_FN(AssetECS, setSystemRate     , ev_assetecs_setsystemrate);
  EV_NS_BIND_FN(AssetECS, setOnAddTrigger   , ev_assetecs_setonaddtrigger);
  EV_NS_BIND_FN(AssetECS, setOnRemoveTrigger, ev_assetecs_setonremovetrigger);
  EV_NS_BIND_FN(AssetECS, setOnSetTrigger   , ev_assetecs_setonsettrigger);
  EV_NS_BIND_FN(AssetECS, forEachChild      , ev_assetecs_foreachchild);
  EV_NS_BIND_FN(AssetECS, getParent         , ev_assetecs_getparent);
  EV_NS_BIND_FN(AssetECS, setSingleton      , ev_assetecs_setsingleton);
  EV_NS_BIND_FN(AssetECS, getSingleton      , ev_assetecs_getsingleton);
  EV_NS_BIND_FN(AssetECS, modified          , ev_assetecs_modified);
  EV_NS_BIND_FN(AssetECS, createChildEntity , ev_assetecs_createchildentity);
  EV_NS_BIND_FN(AssetECS, addChildToEntity  , ev_assetecs_addchildtoentity);
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
/* ev_ecs_init_pipeline() */
/* { */
/* #define STAGE(name) EV_CONCAT(EV_ECS_PIPELINE_STAGE_, name) */
/* #define STAGE_IMPL(name) EV_CONCAT(STAGE(name), _IMPL) */
/* #define STAGE_IMPL_STR(name) EV_STRINGIZE(STAGE_IMPL(name)) */

/*   // Registering stage tags in the world */
/* #define PIPELINE_OP(stage) \ */
/*   ECSEntityID STAGE_IMPL(stage) = ev_ecs_registertag(STAGE_IMPL_STR(stage)); */
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
