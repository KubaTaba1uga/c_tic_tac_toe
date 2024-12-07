root = join_paths('..', '..')
src = join_paths(root, 'src')
utils = join_paths(src, 'utils')
input = join_paths(src, 'input')
init = join_paths(src, 'init')
config = join_paths(src, 'config')
display = join_paths(src, 'display')
game = join_paths(src, 'game')
game_state_machine = join_paths(game, 'game_state_machine')
game_sub_sms = join_paths(game_state_machine, 'sub_state_machines')
user_move = join_paths(game_state_machine, 'user_move')

############################################################################
#                   Game State Machine Subsystem Tests                     #
############################################################################
test_cli_display_name = 'test_cli_display.c'

test_cli_display_src = files([test_cli_display_name]) + sources

test_cli_display_exe = executable('test_cli_display',
  sources: [
    test_cli_display_src,
    unity_gen_runner.process(test_cli_display_name),
  ],
  include_directories: [src, test_includes],
  dependencies: test_dependencies,
  c_args:['-DTEST'],
  link_args: [
    # allow static functions testing
    '-zmuldefs',
  ]
)

test('test_cli_display', test_cli_display_exe)
