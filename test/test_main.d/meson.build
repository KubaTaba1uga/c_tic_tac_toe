root = join_paths('..', '..')
src = join_paths(root, 'src')

test_main_name = 'test_main.c'

test_main_src = files([test_main_name]) + sources

test_main_exe = executable('test_main',
  sources: [
    test_main_src,
    unity_gen_runner.process(test_main_name),
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
