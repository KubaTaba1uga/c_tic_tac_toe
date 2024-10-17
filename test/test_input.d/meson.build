############################################################################
#                   Keyboard Tests                                         #
############################################################################
root = join_paths('..', '..')
src = join_paths(root, 'src')
utils = join_paths(src, 'utils')
input = join_paths(src, 'input')
keyboard = join_paths(input, 'keyboard')
init = join_paths(src, 'init')

test_keyboard_name = 'test_keyboard.c'

test_keyboard_src = files([test_keyboard_name,
			   keyboard / 'keyboard.c',
			   keyboard / 'keyboard1.c', 
			   utils / 'logging_utils.c', 
			   utils / 'std_lib_utils.c', 
			   input / 'input.c', 
			   init / 'init.c'])

test_keyboard_exe = executable('test_keyboard',
  sources: [
    test_keyboard_src,
    cmock_gen_runner.process(test_keyboard_name),
  ],
  include_directories: [src],
  dependencies: test_dependencies,
  c_args:['-DTEST'],
  link_args: [
    # allow static functions testing
    '-zmuldefs',
  ]
)

test('test_keyboard', test_keyboard_exe)

############################################################################
#                   Keyboard1 Tests                                        #
############################################################################
test_keyboard1_name = 'test_keyboard1.c'

test_keyboard1_src = files([test_keyboard1_name, 
			   keyboard / 'keyboard1.c', 
			   keyboard / 'keyboard.c', 
			   utils / 'logging_utils.c', 
			   utils / 'std_lib_utils.c', 
			   input / 'input.c', 
			   init / 'init.c'])

test_keyboard1_exe = executable('test_keyboard1',
  sources: [
    test_keyboard1_src,
    cmock_gen_runner.process(test_keyboard1_name),
  ],
  include_directories: [src],
  dependencies: test_dependencies,
  c_args:['-DTEST'],
  link_args: [
    # allow static functions testing
    '-zmuldefs',
  ]
)

test('test_keyboard1', test_keyboard1_exe)
