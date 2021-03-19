project('template', 'c',
version : '0.1',
default_options : ['warning_level=3'])

cc = meson.get_compiler('c')

mod_src = [
  'src/template.c'
]

mod_incdir = [ '..' ]

evol_dep = dependency('evol')
dl_dep = cc.find_library('dl', required: false)

mod_deps = [
  evol_dep,
  dl_dep,
]

moduleconfig = get_option('moduleconfig')
configure_file(
  input: moduleconfig,
  command: [
    meson.project_source_root() + '/subprojects/luajit/src/luajit', 
    meson.project_source_root() + '/subprojects/evol/buildscripts/bin2cstring.lua', 
    meson.project_source_root() + '/' + moduleconfig
  ],
  output: moduleconfig + '.h',
  capture: true
)

module = shared_module(
  'template', mod_src,
  include_directories: mod_incdir,
  dependencies: mod_deps,
)

mod_dep = declare_dependency(
  include_directories: mod_incdir,
)

if meson.version().version_compare('>= 0.54.0')
  meosn.override_dependency('evmod_template', mod_dep)
endif
