project('wren', 'c', version: '0.4.0', default_options: ['c_std=c99'])

cc = meson.get_compiler('c')

libm = cc.find_library('m')
test_cmd = find_program('test')
egrep_cmd = find_program('egrep')
find_cmd = find_program('find')

src = 'src'
include = src / 'include'
optional = src / 'optional'
vm = src / 'vm'

source = files(
    optional / 'wren_opt_meta.c',
    optional / 'wren_opt_random.c',
    vm / 'wren_compiler.c',
    vm / 'wren_core.c',
    vm / 'wren_debug.c',
    vm / 'wren_primitive.c',
    vm / 'wren_utils.c',
    vm / 'wren_value.c',
    vm / 'wren_vm.c'
)

includes = include_directories(include, vm, optional)

lib = both_libraries('wren', source,
    include_directories: includes,
    dependencies: [libm],
    install: true)

install_headers(include / 'wren.h', include / 'wren.hpp')

pkg = import('pkgconfig')

libwren_dep = declare_dependency(include_directories: include_directories(include), link_with: lib.get_static_lib())

pkg.generate(lib)

test = 'test'
api = test / 'api'

api_tests = run_command(find_cmd, 'test/api', '-name', '*.wren').stdout().strip().split('\n')

other_tests = run_command(find_cmd, 'test', '-path', 'test/api', '-prune', '-o', '-name', '*.wren').stdout().strip().split('\n')

test_source = files(
    test / 'main.c',
    test / 'test.c',
    api / 'api_tests.c',
    api / 'benchmark.c'
)

foreach t : api_tests
    c_file = t.split('.')[0] + '.c'
    if run_command(test_cmd, '-f', c_file).returncode() == 0
        test_source += c_file
    endif
endforeach

test_includes = include_directories(include)

wren_test = executable('wren_test', test_source,
    link_with: [lib],
    include_directories: test_includes)
foreach t : (other_tests + api_tests)
    if t != 'test/api'
        should_fail = run_command(egrep_cmd, 'expect.* error', t).returncode() == 0
        test(t, wren_test, workdir: meson.current_source_dir(), args: [t], should_fail: should_fail)
    endif
endforeach
