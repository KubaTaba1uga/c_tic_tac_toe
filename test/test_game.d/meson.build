root = join_paths('..', '..')
src = join_paths(root, 'src')
utils = join_paths(src, 'utils')
input = join_paths(src, 'input')
init = join_paths(src, 'init')
config = join_paths(src, 'config')
game = join_paths(src, 'game')
game_state_machine = join_paths(game, 'game_state_machine')
user_move = join_paths(game_state_machine, 'user_move')

############################################################################
#                   User Move Tests                                        #
############################################################################
test_user_move_name = 'test_user_move.c'

test_user_move_src = [test_user_move_name, user_move / 'user_move.c', game_state_machine / 'game_state_machine.c', game_state_machine / 'game_sm_subsystem.c', game / 'game.c', utils / 'logging_utils.c', utils / 'std_lib_utils.c', input / 'input.c', init / 'init.c', config / 'config.c']

test_user_move_exe = executable('test_user_move',
  sources: [
    test_user_move_src,
    cmock_gen_runner.process(test_user_move_name),
  ],
  include_directories: [src],
  dependencies: test_dependencies,
  c_args:['-DTEST'],
  link_args: [
    # allow static functions testing
    '-zmuldefs',
  ]
)

test('test_user_move', test_user_move_exe)

############################################################################
#                   Quit State Machine Tests                               #
############################################################################
test_quit_sm_name = 'test_quit_sm.c'

test_quit_sm_src = [test_quit_sm_name,
		 user_move / 'user_move.c',
		 game_state_machine / 'game_state_machine.c',
		 game_state_machine / 'game_sm_subsystem.c',
		 game / 'game.c',
		 utils / 'logging_utils.c',
		 utils / 'std_lib_utils.c',
		 input / 'input.c',
		 init / 'init.c',
		 config / 'config.c']

test_quit_sm_exe = executable('test_quit_sm',
  sources: [
    test_quit_sm_src,
    cmock_gen_runner.process(test_quit_sm_name),
  ],
  include_directories: [src],
  dependencies: test_dependencies,
  c_args:['-DTEST'],
  link_args: [
    # allow static functions testing
    '-zmuldefs',
  ]
)

test('test_quit_sm', test_quit_sm_exe)
