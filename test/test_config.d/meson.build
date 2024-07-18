root = join_paths('..', '..')
# src = join_paths(root, 'src')
utils = join_paths(src, 'utils')
config = join_paths(src, 'config')
init = join_paths(src, 'init')

test_config_name = 'test_config.c'

test_config_src = files([test_config_name, config / 'config.c', utils / 'logging_utils.c', init / 'init.c'])

test_config_exe = executable('test_config',
  sources: [
    test_config_src,
    cmock_gen_runner.process(test_config_name),
  ],
  include_directories: [src],
  dependencies: test_dependencies,
  c_args:['-DTEST'],
  link_args: [
    # allow static functions testing
    '-zmuldefs',
  ]
)

test('test_config', test_config_exe)
