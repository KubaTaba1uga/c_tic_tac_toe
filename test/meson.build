cmock_subproject = subproject('cmock')

unity_dependency = cmock_subproject.get_variable('unity_dep')
cmock_dependency = cmock_subproject.get_variable('cmock_dep')

test_dependencies = [logging_dep, unity_dependency, cmock_dependency]

scripts = join_paths('..', 'scripts')

cmock_gen_runner = generator(
  find_program(scripts / 'create_runner.rb'),
  output: '@BASENAME@_Runner.c',
  arguments: ['@INPUT@', '@OUTPUT@'],
)

cmock_gen_mock = generator(
   find_program(scripts / 'create_mock.rb'),
   output: ['mock_@BASENAME@.c', 'mock_@BASENAME@.h', '@PLAINNAME@'],
   arguments: ['@INPUT@', '@BUILD_DIR@'],
)

subdir('test_gardener.d')
subdir('test_cmock.d')
subdir('test_main.d')
subdir('test_logging_utils.d')
subdir('test_config.d')