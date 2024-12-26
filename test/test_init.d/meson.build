root = join_paths('..', '..')
src = join_paths(root, 'src')
utils = join_paths(src, 'utils')
init = join_paths(src, 'init')
config = join_paths(src, 'config')
input = join_paths(src, 'input')
game = join_paths(src, 'game')

test_init_name = 'test_init.c'

test_init_src = [test_init_name,
                 init / 'init.c',
		 config / 'config.c',
		 input / 'input.c',
		 input / 'input_device.c',		 
		 input / 'keyboard' / 'keyboard.c',
                 input / 'keyboard' / 'keyboard_keys_mapping.c',
                 input / 'keyboard' / 'keyboard_keys_mapping_1.c',
		 utils / 'terminal_utils.c',		 
		 utils / 'std_lib_utils.c',
		 utils / 'logging_utils.c',
		 game / 'game.c',
		 ]

test_init_exe = executable('test_init',
  sources: [
    test_init_src,
    unity_gen_runner.process(test_init_name),
  ],
  include_directories: test_includes,
  dependencies: test_dependencies,
  c_args:['-DTEST'],
)

test('test_init', test_init_exe)
