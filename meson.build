project('evol-mod-flecs', 'c',
version : '0.2',
default_options : ['warning_level=3', 'c_std=c11'])

src = subproject('evol').get_variable('evolmod_subdir') + '/meson.build'
dst = meson.current_source_dir() + '/import/'
run_command( 'python3', meson.source_root() + '/subprojects/evol/buildscripts/copy.py', src, dst)
subdir('import')

flecs_dep = dependency('flecs')

mod_src = [
  'src/mod.c',
  'src/ecs/os_api.c',
  'src/ecs/common_ecs.c',
  'src/ecs/game_ecs.c',
  'src/ecs/asset_ecs.c',
]

mod_incdir = [
  '..', 
  'include',
]

mod_deps = [
  evmod_deps,

  flecs_dep,
]

module = shared_module(
  'flecs', mod_src,
  include_directories: mod_incdir,
  dependencies: mod_deps,
  name_prefix: evmod_name_prefix,
  name_suffix: evmod_name_suffix,
)

mod_dep = declare_dependency(
  include_directories: mod_incdir,
)

meson.override_dependency('evmod_ecs', mod_dep)
