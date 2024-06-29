root = join_paths('..', '..')
src = join_paths(root, 'src')
utils = join_paths(src, 'utils')
init = join_paths(src, 'init')

test_main_name = 'test_main.c'

test_main_src = files([test_main_name, src / 'main.c', utils / 'std_lib_utils.c', utils / 'logging_utils.c', init / 'init.c'])

test_main_exe = executable('test_main',
  sources: [
    test_main_src,
    cmock_gen_runner.process(test_main_name),
  ],
  include_directories: [src],
  dependencies: test_dependencies,
  c_args:['-DTEST'],
  link_args: [
    # allow static functions testing
    '-zmuldefs',
  ]
)

test('test_main', test_main_exe)
