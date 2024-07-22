root = join_paths('..', '..')
# src = join_paths(root, 'src')
utils = join_paths(src, 'utils')
input = join_paths(src, 'input')
init = join_paths(src, 'init')

test_keyboard_name = 'test_keyboard.c'

test_keyboard_src = files([test_keyboard_name, utils / 'logging_utils.c', utils / 'std_lib_utils.c', input / 'input.c', init / 'init.c'])

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
