project('evol-mod-flecs', 'c',
version : '0.1',
default_options : ['warning_level=3', 'c_std=c11'])

# Meson modules import
fs = import('fs')

cc = meson.get_compiler('c')

mod_src = [
  'src/mod.c',
  'src/os_api.c',
]

mod_args = []

# GCC arg for ms-extensions (specifically, nameless anonymous structs/unions)
if build_machine.system() != 'windows'
  mod_args += '-fms-extensions'
endif

# Metafiles c_args defs
if fs.is_file('meta/evmod.namespaces')
  mod_args += '-DEVMOD_NAMESPACES_DEF'
endif
if fs.is_file('meta/evmod.events')
  mod_args += '-DEVMOD_EVENTS_DEF'
endif
if fs.is_file('meta/evmod.types')
  mod_args += '-DEVMOD_TYPES_DEF'
endif

mod_incdir = ['..', 'include']

evol_dep = dependency('evol')

dl_dep = cc.find_library('dl', required: false)

flecs_dep = dependency('flecs')

mod_deps = [
  evol_dep,
  dl_dep,
  flecs_dep,
]

moduleconfig = get_option('moduleconfig')
configure_file(
  input: moduleconfig,
  command: [
    meson.source_root() + '/subprojects/luajit/src/luajit',
    meson.source_root() + '/subprojects/evol/buildscripts/bin2cstring.lua',
    meson.project_source_root() + '/' + moduleconfig
  ],
  output: moduleconfig + '.h',
  capture: true
)
mod_args += '-DEVMOD_LUA='+meson.project_build_root()+'/'+moduleconfig+'.h'

module = shared_module(
  'evmodflecs', mod_src,
  include_directories: mod_incdir,
  dependencies: mod_deps,
  c_args: mod_args,
)

mod_dep = declare_dependency(
  include_directories: mod_incdir,
)

if meson.version().version_compare('>= 0.54.0')
  meson.override_dependency('evmod_ecs', mod_dep)
endif
