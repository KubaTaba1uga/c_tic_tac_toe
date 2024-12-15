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

test_keyboard_src = files([test_keyboard_name]) + sources

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

test_keyboard1_src = files([test_keyboard1_name]) + sources

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
#                   Input Registration Tests                               #
############################################################################
test_input_registration_name = 'test_input_registration.c'

test_input_registration_src = files([test_input_registration_name]) + sources

test_input_registration_exe = executable('test_input_registration',
  sources: [
    test_input_registration_src,
    unity_gen_runner.process(test_input_registration_name),
  ],
  include_directories: [src, test_includes],
  dependencies: test_dependencies,
  c_args:['-DTEST'],
  link_args: [
    # allow static functions testing
    '-zmuldefs',
  ]
)

test('test_input_registration', test_input_registration_exe)
