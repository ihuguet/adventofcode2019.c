project('Advent of Code 2019', 'c')

deps = [dependency('glib-2.0'), dependency('gobject-2.0')]
aoc = static_library('aoc', sources: 'aoc_input.c', dependencies: deps)
test_env = [
    'G_TEST_SRCDIR=@0@'.format(meson.current_source_dir()),
    'G_TEST_BUILDDIR=@0@'.format(meson.current_build_dir()),
]
test_c_args = [
    '-DTEST',
    '-Wno-unused-function'
]

day01 = executable('day01', sources: 'day01.c', link_with: aoc, dependencies: deps)
test01 = executable('test01', sources: 'day01.c', link_with: aoc, dependencies: deps, c_args: test_c_args)
test('day01', test01, env: test_env, protocol: 'tap')

day02 = executable('day02', sources: 'day02.c', link_with: aoc, dependencies: deps)
test02 = executable('test02', sources: 'day02.c', link_with: aoc, dependencies: deps, c_args: test_c_args)
test('day02', test02, env: test_env, protocol: 'tap')

day03 = executable('day03', sources: 'day03.c', link_with: aoc, dependencies: deps)
