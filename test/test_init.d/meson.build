root = join_paths('..', '..')
src = join_paths(root, 'src')
root = join_paths('..', '..')
src = join_paths(root, 'src')
utils = join_paths(src, 'utils')
init = join_paths(src, 'init')
config = join_paths(src, 'config')

test_init_name = 'test_init.c'

test_init_src = [test_init_name,
                 init / 'init.c',
		 config / 'config.c',
		 utils / 'array_utils.c',
		 utils / 'registration_utils.c',
		 utils / 'std_lib_utils.c',
		 utils / 'logging_utils.c']

test_init_exe = executable('test_init',
  sources: [
    test_init_src,
    unity_gen_runner.process(test_init_name),
  ],
  include_directories: [src, test_includes],
  dependencies: test_dependencies,
  c_args:['-DTEST'],
  link_args: [
    # allow static functions testing
    '-zmuldefs',
  ]
)

test('test_init', test_init_exe)
