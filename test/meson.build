unity_subproject = subproject('unity')

unity_dependency = unity_subproject.get_variable('unity_dep')

test_dependencies = [logging_dep, pthread_dep, unity_dependency]
test_includes = include_directories('wrappers')

scripts = join_paths('..', 'scripts')

unity_gen_runner = generator(
  find_program(scripts / 'create_runner.rb'),
  output: '@BASENAME@_Runner.c',
  arguments: ['@INPUT@', '@OUTPUT@'],
)

subdir('test_unity.d')
subdir('test_main.d')
subdir('test_logging_utils.d')
subdir('test_config.d')
subdir('test_input.d')
subdir('test_game.d')
subdir('test_display.d')