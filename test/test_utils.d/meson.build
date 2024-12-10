root = join_paths('..', '..')
src = join_paths(root, 'src')
utils = join_paths(src, 'utils')
init = join_paths(src, 'init')


############################################################################
#                   Logging Utils Tests                                    #
############################################################################
test_logging_utils_name = 'test_logging_utils.c'

test_logging_utils_src = files([test_logging_utils_name]) + sources

test_logging_utils_exe = executable('test_logging_utils',
  sources: [
    test_logging_utils_src,
    unity_gen_runner.process(test_logging_utils_name),
  ],
  include_directories: [src, test_includes],
  dependencies: test_dependencies,
  c_args:['-DTEST'],
  link_args: [
    # allow static functions testing
    '-zmuldefs',
  ]
)

test('test_logging_utils', test_logging_utils_exe)

############################################################################
#                   Array Utils Tests                                    #
############################################################################
test_array_utils_name = 'test_array_utils.c'

test_array_utils_src = [test_array_utils_name, utils / 'array_utils.c']

test_array_utils_exe = executable(
    'test_array_utils',
    sources: [
              test_array_utils_src,
              unity_gen_runner.process(test_array_utils_name),
	     ],
    include_directories: [src, test_includes],
    dependencies: test_dependencies,
    c_args: ['-DTEST'],
    link_args: ['-zmuldefs']
)

test('test_array_utils', test_array_utils_exe)

############################################################################
#                   Subsystem Utils Tests                                    #
############################################################################
test_subsystem_utils_name = 'test_subsystem_utils.c'

test_subsystem_utils_src = files([test_subsystem_utils_name]) + sources

test_subsystem_utils_exe = executable(
    'test_subsystem_utils',
    sources: [
              test_subsystem_utils_src,
              unity_gen_runner.process(test_subsystem_utils_name),
	     ],
    include_directories: [src, test_includes],
    dependencies: test_dependencies,
    c_args: ['-DTEST'],
    link_args: ['-zmuldefs']
)

test('test_subsystem_utils', test_subsystem_utils_exe)
