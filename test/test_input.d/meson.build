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

test_keyboard_src = [test_keyboard_name,
                   keyboard / 'keyboard.c',
                   keyboard / 'keyboard_registration.c',		   
                   input / 'input.c',
                   input / 'input_registration.c',
		   utils / 'std_lib_utils.c',
		   utils / 'logging_utils.c',
   		   utils / 'terminal_utils.c'		
		   ]

test_keyboard_exe = executable('test_keyboard',
  sources: [
    test_keyboard_src,
    unity_gen_runner.process(test_keyboard_name),
  ],
  include_directories: [src, test_includes],
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

test_keyboard1_src = [test_keyboard1_name,
                      keyboard / 'keyboard1.c',
                      keyboard / 'keyboard.c',
                      keyboard / 'keyboard_registration.c',		   
                      input / 'input.c',
                      input / 'input_registration.c',
		      utils / 'std_lib_utils.c',
		      utils / 'logging_utils.c',
   		      utils / 'terminal_utils.c'		
		   ]

test_keyboard1_exe = executable('test_keyboard1',
  sources: [
    test_keyboard1_src,
    unity_gen_runner.process(test_keyboard1_name),
  ],
  include_directories: [src, test_includes],
  dependencies: test_dependencies,
  c_args:['-DTEST'],
  link_args: [
    # allow static functions testing
    '-zmuldefs',
  ]
)

test('test_keyboard1', test_keyboard1_exe)

############################################################################
#                   Input Tests                                        #
############################################################################
test_input_name = 'test_input.c'

test_input_src = [test_input_name,
                   input / 'input.c',
                   input / 'input_registration.c',		   
		   utils / 'std_lib_utils.c',
		   utils / 'logging_utils.c']

test_input_exe = executable('test_input',
  sources: [
    test_input_src,
    unity_gen_runner.process(test_input_name),
  ],
  include_directories: [src, test_includes],
  dependencies: test_dependencies,
  c_args:['-DTEST'],
)

test('test_input', test_input_exe)
