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
#                   Game Config Tests                                      #
############################################################################
test_game_config_name = 'test_game_config.c'

test_game_config_src = [test_game_config_name,
                   game / 'game.c',
                   game / 'game_config.c',
                   game / 'game_user.c',
		   game_state_machine / 'game_state_machine.c',
                   keyboard / 'keyboard.c',
                   keyboard / 'keyboard_keys_mapping.c',
                   keyboard / 'keyboard_keys_mapping_1.c',
		   config / 'config.c',
                   input / 'input.c',
                   input / 'input_device.c',		   
		   utils / 'std_lib_utils.c',
		   utils / 'logging_utils.c',
   		   utils / 'terminal_utils.c'		
		   ]


test_game_config_exe = executable('test_game_config',
  sources: [
    test_game_config_src,
    unity_gen_runner.process(test_game_config_name),
  ],
  include_directories: [src, test_includes],
  dependencies: test_dependencies,
  c_args:['-DTEST'],
)

test('test_game_config', test_game_config_exe)


############################################################################
#                   Game State Machine Subsystem Tests                     #
############################################################################
test_gsm_subsystem_name = 'test_game_state_machine_subsystem.c'

test_gsm_subsystem_src = files([test_gsm_subsystem_name]) + sources

test_gsm_subsystem_exe = executable('test_gsm_subsystem',
  sources: [
    test_gsm_subsystem_src,
    unity_gen_runner.process(test_gsm_subsystem_name),
  ],
  include_directories: [src, test_includes],
  dependencies: test_dependencies,
  c_args:['-DTEST'],
  link_args: [
    # allow static functions testing
    '-zmuldefs',
  ]
)

test('test_gsm_subsystem', test_gsm_subsystem_exe)


# ############################################################################
# #                   User Move State Machine Module Tests                   #
# ############################################################################
# test_user_move_name = 'test_user_move.c'

# test_user_move_src = files([test_user_move_name]) + sources

# test_user_move_exe = executable('test_user_move',
#   sources: [
#     test_user_move_src,
#     unity_gen_runner.process(test_user_move_name),
#   ],
#   include_directories: [src, test_includes],
#   dependencies: test_dependencies,
#   c_args:['-DTEST'],
#   link_args: [
#     # allow static functions testing
#     '-zmuldefs',
#   ]
# )

# test('test_user_move', test_user_move_exe)

# ############################################################################
# #                   Quit State Machine Module Tests                        #
# ############################################################################
# test_quit_sm_name = 'test_quit_sm.c'

# test_quit_sm_src = files([test_quit_sm_name]) + sources

# test_quit_sm_exe = executable('test_quit_sm',
#   sources: [
#     test_quit_sm_src,
#     unity_gen_runner.process(test_quit_sm_name),
#   ],
#   include_directories: [src, test_includes],
#   dependencies: test_dependencies,
#   c_args:['-DTEST'],
#   link_args: [
#     # allow static functions testing
#     '-zmuldefs',
#   ]
# )

# test('test_quit_sm', test_quit_sm_exe)

