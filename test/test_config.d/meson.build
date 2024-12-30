root = join_paths('..', '..')
src = join_paths(root, 'src')
utils = join_paths(src, 'utils')
init = join_paths(src, 'init')
config = join_paths(src, 'config')

test_config_name = 'test_config.c'

test_config_src = [test_config_name,
		   config / 'config.c',
		   init / 'init.c',		   
		   utils / 'std_lib_utils.c',
		   utils / 'logging_utils.c',
		   # Next files are required by init
		 input / 'input.c',
		 input / 'input_device.c',		 
		 input / 'keyboard' / 'keyboard.c',
                 input / 'keyboard' / 'keyboard_keys_mapping.c',
                 input / 'keyboard' / 'keyboard_keys_mapping_1.c',
		 utils / 'terminal_utils.c',		 
		 game / 'game.c',
		 game / 'game_config.c',
		 game / 'game_state_machine' / 'game_state_machine.c',
		 game / 'game_state_machine' / 'game_sm_subsystem.c',		 
  		 game / 'game_state_machine' / 'mini_state_machines' / 'user_move_mini_machine.c',		 		 			          game / 'game_state_machine' / 'mini_state_machines' / 'quit_mini_machine.c',
  		 game / 'game_state_machine' / 'mini_state_machines' / 'moves_cleanup_mini_machine.c',		 		 			          game / 'game_state_machine' / 'mini_state_machines' / 'common.c',
		 ]

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
