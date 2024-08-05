root = join_paths('..', '..')
src = join_paths(root, 'src')
utils = join_paths(src, 'utils')
input = join_paths(src, 'input')
init = join_paths(src, 'init')
config = join_paths(src, 'config')
game = join_paths(src, 'game')
user_move = join_paths(game, 'user_move')

test_user_move_name = 'test_user_move.c'

# test_user_move_src = [sources, test_user_move_name]
test_user_move_src = [test_user_move_name, user_move / 'user_move.c', game / 'game_state_machine.c', utils / 'logging_utils.c', utils / 'std_lib_utils.c', input / 'input.c', init / 'init.c', config / 'config.c']


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
