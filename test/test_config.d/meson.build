test_config_name = 'test_config.c'

test_config_src = files([test_config_name]) + sources

test_config_exe = executable('test_config',
  sources: [
    test_config_src,
    unity_gen_runner.process(test_config_name),
  ],
  include_directories: [src, test_includes],
  dependencies: test_dependencies,
  c_args:['-DTEST'],
  link_args: [
    # allow static functions testing
    '-zmuldefs',
  ]
)

test('test_config', test_config_exe)
