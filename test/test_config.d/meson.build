root = join_paths('..', '..')
src = join_paths(root, 'src')
utils = join_paths(src, 'utils')
init = join_paths(src, 'init')
config = join_paths(src, 'config')

test_config_name = 'test_config.c'

test_config_src = [test_config_name,
		   config / 'config.c',
		   utils / 'std_lib_utils.c',
		   utils / 'logging_utils.c']

test_config_exe = executable('test_config',
  sources: [
    test_config_src,
    unity_gen_runner.process(test_config_name),
  ],
  include_directories: test_includes,
  dependencies: test_dependencies,
  c_args:['-DTEST'],
)

test('test_config', test_config_exe)
