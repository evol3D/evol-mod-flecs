#define EV_MODULE_DEFINE
#include <evol/evolmod.h>

#include <stdio.h>

EV_CONSTRUCTOR {
  printf("Loaded template module\n");
}

U32 _ev_world_update(F32 deltaTime)
{
  EV_UNUSED_PARAM(deltaTime);
  return 0;
}

EV_DESTRUCTOR {
  printf("Unloaded template module\n");
}

EV_BINDINGS
{
  EV_NS_BIND_FN(World, update, _ev_world_update);
}
