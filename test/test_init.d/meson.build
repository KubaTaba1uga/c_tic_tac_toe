root = join_paths('..', '..')
src = join_paths(root, 'src')

test_init_name = 'test_init.c'

test_init_src = files([test_init_name]) + sources

test_init_exe = executable('test_init',
  sources: [
    test_init_src,
    unity_gen_runner.process(test_init_name),
  ],
  include_directories: [src],
  dependencies: test_dependencies,
  c_args:['-DTEST'],
  link_args: [
    # allow static functions testing
    '-zmuldefs',
  ]
)

test('test_init', test_init_exe)
