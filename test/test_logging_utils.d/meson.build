root = join_paths('..', '..')
src = join_paths(root, 'src')
utils = join_paths(src, 'utils')
init = join_paths(src, 'init')

test_logging_utils_name = 'test_logging_utils.c'

test_logging_utils_src = files([test_logging_utils_name, 
			   utils / 'logging_utils.c', 
			   init / 'init.c', 
			   utils / 'std_lib_utils.c'])

test_logging_utils_exe = executable('test_logging_utils',
  sources: [
    test_logging_utils_src,
    cmock_gen_runner.process(test_logging_utils_name),
  ],
  include_directories: [src],
  dependencies: test_dependencies,
  c_args:['-DTEST'],
  link_args: [
    # allow static functions testing
    '-zmuldefs',
  ]
)

test('test_logging_utils', test_logging_utils_exe)
